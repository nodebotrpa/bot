#ifndef DRIVER_HPP
#define DRIVER_HPP

#include <Core/Core.h>
using namespace Upp;

struct keyCodes {
	keyCodes(){
		keym.Add("{CTRL}",WString({0xE009}));
		keym.Add("{NULL}",WString({0xE000}));
		keym.Add("{CANCEL}",WString({0xE001}));
		keym.Add("{HELP}",WString({0xE002}));
		keym.Add("{BACK_SPACE}",WString({0xE003}));
		keym.Add("{TAB}",WString({0xE004}));
		keym.Add("{CLEAR}",WString({0xE005}));
		keym.Add("{RETURN}",WString({0xE006}));
		keym.Add("{ENTER}",WString({0xE007}));
		keym.Add("{SHIFT}",WString({0xE008}));
		keym.Add("{CONTROL}",WString({0xE009}));
		keym.Add("{ALT}",WString({0xE00A}));
		keym.Add("{PAUSE}",WString({0xE00B}));
		keym.Add("{ESCAPE}",WString({0xE00C}));
		keym.Add("{SPACE}",WString({0xE00D}));
		keym.Add("{PAGE_UP}",WString({0xE00E}));
		keym.Add("{PAGE_DOWN}",WString({0xE00F}));
		keym.Add("{END}",WString({0xE010}));
		keym.Add("{HOME}",WString({0xE011}));
		keym.Add("{LEFT}",WString({0xE012}));
		keym.Add("{UP}",WString({0xE013}));
		keym.Add("{RIGHT}",WString({0xE014}));
		keym.Add("{DOWN}",WString({0xE015}));
		keym.Add("{INSERT}",WString({0xE016}));
		keym.Add("{DELETE}",WString({0xE017}));
		keym.Add("{SEMICOLON}",WString({0xE018}));
		keym.Add("{EQUALS}",WString({0xE019}));
		keym.Add("{NUMPAD0}",WString({0xE01A}));
		keym.Add("{NUMPAD1}",WString({0xE01B}));
		keym.Add("{NUMPAD2}",WString({0xE01C}));
		keym.Add("{NUMPAD3}",WString({0xE01D}));
		keym.Add("{NUMPAD4}",WString({0xE01E}));
		keym.Add("{NUMPAD5}",WString({0xE01F}));
		keym.Add("{NUMPAD6}",WString({0xE020}));
		keym.Add("{NUMPAD7}",WString({0xE021}));
		keym.Add("{NUMPAD8}",WString({0xE022}));
		keym.Add("{NUMPAD9}",WString({0xE023}));
		keym.Add("{MULTIPLY}",WString({0xE024}));
		keym.Add("{ADD}",WString({0xE025}));
		keym.Add("{SEPARATOR}",WString({0xE026}));
		keym.Add("{SUBTRACT}",WString({0xE027}));
		keym.Add("{DECIMAL}",WString({0xE028}));
		keym.Add("{DIVIDE}",WString({0xE029}));
		keym.Add("{F1}",WString({0xE031}));
		keym.Add("{F2}",WString({0xE032}));
		keym.Add("{F3}",WString({0xE033}));
		keym.Add("{F4}",WString({0xE034}));
		keym.Add("{F5}",WString({0xE035}));
		keym.Add("{F6}",WString({0xE036}));
		keym.Add("{F7}",WString({0xE037}));
		keym.Add("{F8}",WString({0xE038}));
		keym.Add("{F9}",WString({0xE039}));
		keym.Add("{F10}",WString({0xE03A}));
		keym.Add("{F11}",WString({0xE03B}));
		keym.Add("{F12}",WString({0xE03C}));
		keym.Add("{META}",WString({0xE03D}));
		keym.Add("{ZENKAKU_HANKAKU}",WString({0xE040}));
	}
protected :
	ValueMap keym;
public :
	String getKey(String key) {
		return keym.GetAdd(key).ToString();
	}
};

class driver {
protected :
	keyCodes keys;
	String driverUrl = "";
	String browser;
	HttpRequest http;
	String lastError;
	String lastElementHandle;
	String session;
	bool navigate(String direction);
	ValueMap browserConf;
	bool isResponseOK();
	bool getJsonValue(String name,String content, String &value);
	bool getStringValue(String content, String &value);
	bool getBooleanValue(String content, bool &value);
	bool getNoneValue(String content);
public :
	void setBrowser(String b){
		Value conf;
		browser = b;
		if (IsNull(browserConf[browser])) {
			String s = LoadFile(GetDataFile(browser+".settings"));
			conf = ParseJSON(s);
			if (AsJSON(conf)=="null") {
				MessageBoxA(NULL,"Can not load "+browser+".settings","Driver Error",NULL);
			} else {
				browserConf.Add(browser,conf);
			}
		}
		Value c = browserConf[browser];
		driverUrl = c["url"];
		};
	void setProxy(String host, int port){http.Proxy(host,port);};
	bool useSession(String newSession);
	String getSession(){return session;};
	String getError(){return lastError;};
	String getCapabilities(){Value c  = browserConf[browser]; return AsJSON(c);}
	String getDriverExe(){Value c  = browserConf[browser]; return c["driver"];};
	void setError(String error);
	bool createSession();
	bool deleteSession();
	bool navigateTo(String url);
	bool navigateBack(){return navigate("back");};
	bool navigateForward(){return navigate("forward");};
	bool refresh(){return navigate("refresh");};
	bool getTitle(String &title);
	bool getUrl(String &url);
	bool getAllSessions(String &handle);
	bool setTimeout(String timeout, int seconds);
	bool getTimeout(String timeout, int &seconds);
	bool switchToWindow(String window);
	bool getWindowHandle(String &handle);
	bool closeWindow();
	bool getWindowHandles(String &handles);
	bool switchToFrame(String frame);
	bool switchToParentFrame();
	bool getWindowRect(String &rect);
	bool setWindowRect(int x, int y, int width, int height);
	bool maximize();
	bool minimize();
	bool fullScreen();
	bool findElement(String xpath, String &handle);
	bool findElement(String element, String xpath, String &handle);
	bool findElements(String xpath, String &handle);
	bool findElements(String element, String xpath, String &handle);
	bool getActiveElement(String &handle);
	bool isSelected(String element, bool &status);
	bool isDisplayed(String element, bool &status);
	bool isEnabled(String element, bool &status);
	bool getElementAttribute(String element, String name, String &value);
	bool getElementProperty(String element, String name, String &value);
	bool getElementCss(String element, String name, String &value);
	bool getElementText(String element, String &value);
	bool getElementTag(String element, String &value);
	bool getElementRect(String element, String &value);
	bool moveto(String element, int x, int y);
	bool click(String element, int button = 0);
	bool doubleClick(String element);
	bool mouseDown(String element, int button = 0);
	bool mouseUp(String element, int button = 0);
	bool clear(String element);
	bool type(String element, String text);
	bool getPageSource(String &value);
	bool executeScript(String script, String &value, bool async = false);
	bool getCookies(String &value);
	bool getCookie(String name, String &value);
	bool deleteCookies();
	bool deleteCookie(String name);
	bool addCookie(String domain, int expiry, bool httpOnly, String name, String path, bool secure, String value);
	bool alert(String type);
	bool getAlertText(String &value);
	bool sendAlert(String text);
	bool takeScreenShot(String &value);
	bool takeScreenShot(String element, String &value);
	bool waitUntilPageLoad(bool wait = true);
	bool waitUntilVisible(String element);
};
#endif
