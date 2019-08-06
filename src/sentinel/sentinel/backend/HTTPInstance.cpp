#include "sentinel/backend/HTTPInstance.h"

#include <condition_variable>
#include <include/base/cef_bind.h>
#include <include/cef_browser.h>
#include <include/cef_client.h>
#include <include/wrapper/cef_closure_task.h>
#include <include/wrapper/cef_helpers.h>
#include <future>
#include <mutex>
#include <thread>

namespace sentinel
{
namespace
{

class InternalBrowserHandler: public CefClient,
                              public CefRenderHandler,
                              public CefLoadHandler
{
public:
    InternalBrowserHandler()
    {
        clearResult();
    }
        
    CefRefPtr<CefRenderHandler> GetRenderHandler() override
    {
        return this;
    }

    CefRefPtr<CefLoadHandler> GetLoadHandler() override
    {
        return this;
    }

    void OnPaint(
        CefRefPtr<CefBrowser> browser,
        CefRenderHandler::PaintElementType type,
        const CefRenderHandler::RectList& dirtyRects,
        const void* buffer,
        int width,
        int height) override
    {
    }

    void GetViewRect(
        CefRefPtr<CefBrowser> browser,
        CefRect& rect) override
    {
        rect = CefRect(0, 0, 800, 600);
    }

    void OnLoadEnd(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        int httpStatusCode) override
    {
        if (!frame->IsMain()) {
            return;
        }

        class HTMLVisitor: public CefStringVisitor
        {
        public:
            HTMLVisitor(std::string& result, bool& flag, std::condition_variable& flagCv):
                _result(result),
                _flag(flag),
                _flagCv(flagCv)
            {}

            void Visit(const CefString& str) override
            {
                _result = str;
                _flag = true;
                _flagCv.notify_all();
            }

        private:
            IMPLEMENT_REFCOUNTING(HTMLVisitor);

            std::string& _result;
            bool& _flag;
            std::condition_variable& _flagCv;
        };

        CefRefPtr<HTMLVisitor> visitor(new HTMLVisitor(_result, _hasResult, _resultCv));
        browser->GetMainFrame()->GetSource(visitor);
    }

    void clearResult()
    {
        _hasResult = false;
        _result = "";
    }

    std::string getResult()
    {
        if (!_hasResult) {
            std::mutex m;
            std::unique_lock<std::mutex> lock(m);
            _resultCv.wait(lock);
        }
        assert(_hasResult);
        return _result;
    }

private:
    IMPLEMENT_REFCOUNTING(InternalBrowserHandler);

    std::string _result;
    bool _hasResult{false};
    std::condition_variable _resultCv;
};

}

class HTTPInstanceImpl
{
public:
    explicit HTTPInstanceImpl(const URI& uri);
    ~HTTPInstanceImpl();

    void refresh();
    std::string getResult() const;

private:
    void loadBrowser();
    void loadBrowserTask();
    void waitForBrowser() const;

    URI _uri;
    CefRefPtr<CefBrowser> _browser;
    CefRefPtr<InternalBrowserHandler> _handler;

    // Notify when the browser/handler is successfully created.
    bool _didLoad{false};
    mutable std::condition_variable _browserCv;
};

HTTPInstanceImpl::HTTPInstanceImpl(const URI& uri):
    _uri(uri)
{
    loadBrowser();
}

HTTPInstanceImpl::~HTTPInstanceImpl()
{
    waitForBrowser();

    if (_browser) {
        struct CloseBrowserTask : CefTask
        {
            CloseBrowserTask(CefRefPtr<CefBrowser> browser):
                _browser(browser)
            {}

            void Execute() override
            {
                CEF_REQUIRE_UI_THREAD();
               // _browser->GetHost()->CloseBrowser(true);
            }

        private:
            CefRefPtr<CefBrowser> _browser;
            IMPLEMENT_REFCOUNTING(CloseBrowserTask);
        };
        CefPostTask(TID_UI, new CloseBrowserTask(_browser));
    }
}

void
HTTPInstanceImpl::loadBrowser()
{
    struct LoadTask : CefTask
    {
        LoadTask(HTTPInstanceImpl* impl):
            _impl(impl)
        {}

        void Execute() override
        {
            CEF_REQUIRE_UI_THREAD();
            _impl->loadBrowserTask();
        }

    private:
        IMPLEMENT_REFCOUNTING(LoadTask);
        HTTPInstanceImpl* _impl;
    };

    CefPostTask(TID_UI, new LoadTask(this));
}

void
HTTPInstanceImpl::loadBrowserTask()
{
    _handler = new InternalBrowserHandler();

    // Specify CEF browser settings here.
    CefWindowInfo info;
    info.SetAsWindowless(nullptr);

    CefBrowserSettings browserSettings;
    _browser = CefBrowserHost::CreateBrowserSync(
        info,
        _handler.get(),
        _uri.uri(),
        browserSettings,
        nullptr,
        nullptr);

    _didLoad = true;
    _browserCv.notify_all();
}

void
HTTPInstanceImpl::waitForBrowser() const
{
    if (!_didLoad) {
        std::mutex m;
        std::unique_lock<std::mutex> lock(m);
        _browserCv.wait(lock);
    }
    assert(_didLoad);
    assert(_browser != nullptr);
    assert(_handler != nullptr);
}

void
HTTPInstanceImpl::refresh()
{
    waitForBrowser();
    _handler->clearResult();
    CefPostTask(TID_UI, base::Bind(&CefBrowser::ReloadIgnoreCache, _browser.get()));
}

std::string
HTTPInstanceImpl::getResult() const
{
    waitForBrowser();
    return _handler->getResult();
}

HTTPInstance::HTTPInstance(const URI& uri)
{
    _impl = std::make_shared<HTTPInstanceImpl>(uri);
}

void
HTTPInstance::refresh()
{
    _impl->refresh();
}

std::string
HTTPInstance::getResult() const
{
    return _impl->getResult();
}

}
