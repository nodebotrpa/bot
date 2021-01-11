#include "driver.hpp"
#include "errorcodes.hpp"
#include <plugin/pcre/Pcre.h>
/*
**
** Creates new sssion. If current top-level browser is no longer open,
** returns no such window error. Quits the browser if it's the last window currently open.
** If there is a user prompt, returns its value as an error
**
** @return true if closes window successfully, false otherwise
**
** NBR-08-01-21
**
*/

bool driver::createSession(){
	http.Header("Content-Type","application/json");
	http.Url(driverUrl+"/session");
	http.Post(getCapabilities());
	String content = http.Execute();
	String dirs;
	if (http.GetError() != 0) {
		char dir[512];
		GetCurrentDirectory(512,dir);//GetModuleFileName
		dirs<<dir;
		dirs.Cat("\\driver");
		ShellExecute(NULL,"open",getDriverExe(),NULL,dirs,SW_HIDE);
		content = http.Execute();
	}
	if (http.GetError() != 0) {
		lastError = WEB_NO_DRIVER;
		MessageBox(NULL,"Can not start "+getDriverExe()+" at "+dirs,"Error",MB_ICONERROR);
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"CreateSession:"+content+"\n");
			Value b = ParseJSON(content);
			session = b["sessionId"];
			if (session == "") {
				Value v = b["value"];
				session = v["sessionId"];
			}
			//LOG"SessionId:"+session+"\n");
		} else {
			if (http.GetStatusCode() >= 400) {
				String a = http.GetContent();
				Value err =  ParseJSON(a);
				Value msg = err["value"];
				String errmsg = msg["message"];
				lastError = errmsg;
				return false;
			}
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
/*
**
** Deletes the current session. If current top-level browser is no longer open,
** returns no such window error. Quits the browser if it's the last window currently open.
** If there is a user prompt, returns its value as an error
**
** @return true if closes window successfully, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::deleteSession(){
	http.Url(driverUrl+"/session/"+session);
	http.DEL();
	String content = http.Execute();
	return getNoneValue(content);
}
/*
**
** Navigates to given url. If current top-level browser is no longer open,
** returns no such window error. Quits the browser if it's the last window currently open.
** If there is a user prompt, returns its value as an error
**
** @param url Navigation url
** @return true if closes window successfully, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::navigateTo(String url){
	http.Url(driverUrl+"/session/"+session+"/url");
	Json Data;
	Data("url",url);
	http.Post(Data.ToString());
	String content = http.Execute();
	return getNoneValue(content);
}
/*
**
** Navigates to forward/back. If current top-level browser is no longer open,
** returns no such window error. Quits the browser if it's the last window currently open.
** If there is a user prompt, returns its value as an error
**
** @param direction Navigation forward or back
** @return true if closes window successfully, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::navigate(String direction){
	http.Url(driverUrl+"/session/"+session+"/"+direction);
	http.Post("{}");
	String content = http.Execute();
	return getNoneValue(content);
}
/*
**
** Retrieves all session handles. If current top-level browser is no longer open,
** returns no such window error. Quits the browser if it's the last window currently open.
** If there is a user prompt, returns its value as an error
**
** @param handle Returns all session handles as json
** @return true if closes window successfully, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::getAllSessions(String &handle){
	http.Url(driverUrl+"/sessions");
	http.Get();
	String content = http.Execute();
	return getJsonValue("sessions",content,handle);
}
/*
**
** Retrieves title of active browser window. If current top-level browser is no longer open,
** returns no such window error. Quits the browser if it's the last window currently open.
** If there is a user prompt, returns its value as an error
**
** @param title Returns window title
** @return true if closes window successfully, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::getTitle(String &title){
	http.Url(driverUrl+"/session/"+session+"/title");
	http.GET();
	String content = http.Execute();
	return getStringValue(content,title);
}
/*
**
** Gets current url of browser. If current top-level browser is no longer open,
** returns no such window error. Quits the browser if it's the last window currently open.
** If there is a user prompt, returns its value as an error
**
** @param url Returns url address
** @return true if closes window successfully, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::getUrl(String &url){
	http.Url(driverUrl+"/session/"+session+"/url");
	http.GET();
	String content = http.Execute();
	return getStringValue(content,url);
}
/*
**
** Sets timeout value as miliseconds. If current top-level browser is no longer open,
** returns no such window error. Quits the browser if it's the last window currently open.
** If there is a user prompt, returns its value as an error
**
** @param timeout The type of timeout (implicit, script, pageLoad)
** @param seconds Returns timeout value as miliseconds
** @return true if closes window successfully, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::getTimeout(String timeout, int &seconds) {
	http.Url(driverUrl+"/session/"+session+"/timeouts");
	http.GET();
	String content = http.Execute();
	String value = "";
	bool result = getJsonValue("timeouts",content,value);
	if (result) {
		Value t = ParseJSON(value);
		Value ts = t["timeouts"];
		seconds = ts[timeout];
	}
	return result;
}

/*
**
** Sets timeout value. If current top-level browser is no longer open,
** returns no such window error. Quits the browser if it's the last window currently open.
** If there is a user prompt, returns its value as an error
**
** @param timeout The type of timeout (implicit, script, pageLoad)
** @param seconds Timeout value as seconds
** @return true if closes window successfully, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::setTimeout(String timeout, int seconds){
	http.Url(driverUrl+"/session/"+session+"/timeouts");
	if (seconds<100) seconds = seconds*1000;
	Json Data;
	Data(timeout,seconds);
	http.Post(Data.ToString());
	String content = http.Execute();
	return getNoneValue(content);
}
/*
**
** Retrieves current window handle. If current top-level browser is no longer open,
** returns no such window error. Quits the browser if it's the last window currently open.
** If there is a user prompt, returns its value as an error
**
** @return true if closes window successfully, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::getWindowHandle(String &handle){
	http.Url(driverUrl+"/session/"+session+"/window");
	http.GET();
	String content = http.Execute();
	return getStringValue(content,handle);
}

/*
**
** Closes the current top-level browser. If current top-level browser is no longer open,
** returns no such window error. Quits the browser if it's the last window currently open.
** If there is a user prompt, returns its value as an error
**
** @return true if closes window successfully, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::closeWindow(){
	http.Url(driverUrl+"/session/"+session+"/window");
	http.DEL();
	String content = http.Execute();
	return getNoneValue(content);
}

/*
**
** Switching window will select the top-level browser and commands will be send to the
** windows with given handle. If the handle is undefined, returns invalid argument error.
** If there is an active user prompt, returns unexpected alert open.
**
** @param window The name of the window or the handle of the window
** @return true if switches to window successfully, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::switchToWindow(String window){
	http.Url(driverUrl+"/session/"+session+"/window");
	Json Data;
	Data("handle",window)("name",window);
	http.Post(Data.ToString());
	String content = http.Execute();
	return getNoneValue(content);
}
/*
**
** Retrieves all window handles of browser
** If current top-level browser is no longer open, returns no such window error.
** If there is a browser alert, returns error
**
** @param handles Returns windows handles as Json list
** @return true if http request success, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::getWindowHandles(String &handles){
	http.Url(driverUrl+"/session/"+session+"/window/handles");
	http.GET();
	String content = http.Execute();
	return getStringValue(content, handles);
}
/*
**
** Change focus to the given frame
** If current top-level browser is no longer open, returns no such window error.
** If there is a browser alert, returns error
**
** @param frame handle of the frame
** @return true if http request success, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::switchToFrame(String frame){
	http.Url(driverUrl+"/session/"+session+"/frame");
	Json Data, Element;
	Element("ELEMENT",frame)(lastElementHandle,frame);
	Data("id",Element);
	if (frame == "null") http.Post("{ \"id\" : null }");
	else http.Post(Data.ToString());
	String content = http.Execute();
	return getNoneValue(content);
}
/*
**
** Change focus to the parent context.
** If current top-level browser is no longer open, returns no such window error.
** If there is a browser alert, returns error
**
** @return true if http request success, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::switchToParentFrame(){
	http.Url(driverUrl+"/session/"+session+"/frame/parent");
	http.Post("{}");
	String content = http.Execute();
	return getNoneValue(content);
}
/*
**
** Retrieves the size and position of the current browser window
** If current top-level browser is no longer open, returns no such window error.
** 
** @param rect Returns window coordinates as json
** @return true if http request success, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::getWindowRect(String &rect){
	http.Url(driverUrl+"/session/"+session+"/window/rect");
	http.GET();
	String content = http.Execute();
	return getStringValue(content,rect);
}
/*
**
** Alter the size and position of the current browser window
** If current top-level browser is no longer open, returns no such window error.
** 
** @param x x-coor of window
** @param y y-coor of window
** @param width window width
** @param height window height
** @return true if http request success, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::setWindowRect(int x, int y, int width, int height){
	http.Url(driverUrl+"/session/"+session+"/window/rect");
	Json Data;
	Data("x",x)("y",y)("width",width)("height",height);
	http.Post(Data.ToString());
	String content = http.Execute();
	return getNoneValue(content);
}
/*
**
** Minimizes the current browser window
** If current top-level browser is no longer open, returns no such window error.
** 
** @return true if http request success, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::minimize(){
	http.Url(driverUrl+"/session/"+session+"/window/minimize");
	http.Post("{}");
	String content = http.Execute();
	return getNoneValue(content);
}
/*
**
** Maximizes the current browser window
** If current top-level browser is no longer open, returns no such window error.
** 
** @return true if http request success, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::maximize(){
	http.Url(driverUrl+"/session/"+session+"/window/maximize"); // window/current/maximize
	http.Post("{}");
	String content = http.Execute();
	return getNoneValue(content);
}
/*
**
** Makes full screen the current browser window
** If current top-level browser is no longer open, returns no such window error.
** 
** @return true if http request success, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::fullScreen(){
	http.Url(driverUrl+"/session/"+session+"/window/fullscreen");
	http.Post("{}");
	String content = http.Execute();
	return getNoneValue(content);
}
/*
**
** Retrieves element, starting from the identified element.
** If current top-level browser is no longer open, returns no such window error.
** 
** @param xpath Xpath query
** @param Returns matched element handle as json
** @return true if http request success, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::findElement(String xpath, String &handle){
	http.Url(driverUrl+"/session/"+session+"/element");
	Json Data;
	Data("using","xpath")("value",xpath);
	http.Post(Data.ToString());
	String content = http.Execute();
	String value = "";
	bool result = getJsonValue("element",content,value);
	if (result) {
		Value els = ParseJSON(value);
		Value el = els["element"];
		handle = el["ELEMENT"];
		if (handle.GetCount()==0) {
			ValueMap el2 = els["element"];
			handle = el2.GetValue(0);
			lastElementHandle = el2.GetKey(0);
		}
	}
	return result;
}
/*
**
** Retrieves multiple elements, starting document root.
** If current top-level browser is no longer open, returns no such window error.
** 
** @param xpath Xpath query
** @param Returns matched elements handle as json
** @return true if http request success, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::findElements(String xpath, String &handle){
	http.Url(driverUrl+"/session/"+session+"/elements");
	Json Data;
	Data("using","xpath")("value",xpath);
	http.Post(Data.ToString());
	String content = http.Execute();
	return getJsonValue("elements",content, handle);
}
/*
**
** Retrieves single element, starting from the identified element.
** If current top-level browser is no longer open, returns no such window error.
** 
** @param element Starting element
** @param xpath Xpath query
** @param Returns matched element handle as json
** @return true if http request success, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::findElement(String element, String xpath, String &handle){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/element");
	Json Data;
	Data("using","xpath")("value",xpath);
	http.Post(Data.ToString());
	String content = http.Execute();
	String value = "";
	bool result = getJsonValue("element",content,value);
	if (result) {
		Value els = ParseJSON(value);
		Value el = els["element"];
		handle = el["ELEMENT"];
		if (handle.GetCount() == 0) {
			ValueMap el2 = els["element"];
			handle = el2.GetValue(0);
			lastElementHandle = el2.GetKey(0);
		}
	}
	return result;
}
/*
**
** Retrieves multiple elements, starting from the identified element.
** If current top-level browser is no longer open, returns no such window error.
** 
** @param element Starting element
** @param xpath Xpath query
** @param Returns matched elements handle as json
** @return true if http request success, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::findElements(String element, String xpath, String &handle){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/elements");
	Json Data;
	Data("using","xpath")("value",xpath);
	http.Post(Data.ToString());
	String content = http.Execute();
	return getJsonValue("elements",content,handle);
}
/*
**
** Returns the handle of active/focused element. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error.
** 
** @param handle Returns handle of web element
** @return true if http request success, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::getActiveElement(String &handle){
	http.Url(driverUrl+"/session/"+session+"/element/active");
	http.GET();
	String content = http.Execute();
	return getStringValue(content,handle);
}
/*
**
** Returns the attribute of the given element. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error.
** 
** @param element The id of web element
** @param name The name of the attribute
** @param value Returns attribute value
** @return true if http request success, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::getElementAttribute(String element, String name, String &value){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/attribute/"+name);
	http.GET();
	String content = http.Execute();
	return getStringValue(content,value);
}
/*
**
** Returns the property of the given element. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error.
** 
** @param element The id of web element
** @param name The name of the property
** @param value Returns property value
** @return true if http request success, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::getElementProperty(String element, String name, String &value){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/property/"+name);
	http.GET();
	String content = http.Execute();
	return getStringValue(content,value);
}
/*
**
** Returns the css property of the given element. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error.
** 
** @param element The id of web element
** @param name The name of the CSS property
** @param value Returns css property value
** @return true if http request success, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::getElementCss(String element, String name, String &value){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/css/"+name);
	http.GET();
	String content = http.Execute();
	return getStringValue(content,value);
}
/*
**
** Returns the rendered text of the given element. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error.
** 
** @param element The id of web element
** @param value Returns element's text
** @return true if http request success, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::getElementText(String element, String &value){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/text");
	http.GET();
	String content = http.Execute();
	return getStringValue(content,value);
}
/*
**
** Returns the tag name of the given element. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error.
** 
** @param element The id of web element
** @param value Returns element tag name
** @return true if http request success, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::getElementTag(String element, String &value){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/name");
	http.GET();
	String content = http.Execute();
	return getStringValue(content,value);
}
/*
**
** Returns the dimension and coordinates of the given element. If current top-level
** browser is no longer open, returns no such window error. If there is a browser alert,
** returns error.
** 
** @param element The id of web element
** @param value Returns element x,y,width,height as json
** @return true if http request success, false otherwise
**
** NBR-08-01-21
**
*/
bool driver::getElementRect(String element, String &value){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/rect");
	http.GET();
	String content = http.Execute();
	return getStringValue(content,value);
}
/*
**
** Determine if given element is selected. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error. This operation can use only for Checkbox and Radio Buttons
** 
** @param element The id of web element
** @param status Returns element status
** @return true if http request success, false otherwise
**
** NBR-07-01-21
**
*/
bool driver::isSelected(String element, bool &status){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/selected");
	http.GET();
	String content = http.Execute();
	return getBooleanValue(content,status);
}
/*
**
** Determine if an element is visible on screen. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error.
** 
** @param element The id of web element
** @param status Returns element status
** @return true if http request success, false otherwise
**
** NBR-07-01-21
**
*/
bool driver::isDisplayed(String element, bool &status){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/displayed");
	http.GET();
	String content = http.Execute();
	return getBooleanValue(content,status);
}
/*
**
** Determine if an element is enabled. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error.
** 
** @param element The id of web element
** @param status Returns element status
** @return true if http request success, false otherwise
**
** NBR-07-01-21
**
*/
bool driver::isEnabled(String element, bool &status){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/enabled");
	http.GET();
	String content = http.Execute();
	return getBooleanValue(content,status);
}
/*
**
** Clicks the mouse button on current position. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error. If the element is not interactable returns not interactable error.
** 
** @param element The id of web element
** @param button 0-Left, 1-Middle, 2-Right button
** @return true if http request success, false otherwise
**
** NBR-07-01-21
**
*/
bool driver::click(String element, int button){
	String b;
	http.Url(driverUrl+"/session/"+session+"/actions");
	Json d,el,pt,mm,md,mu;
	el("ELEMENT",element)(lastElementHandle,element);
	mm("duration",100);
	md("type","pointerDown")("button",button);
	mu("type","pointerUp")("button",button);
	b = "{\"actions\":[{\"id\":\"default mouse\",\"type\":\"pointer\",\"parameters\":{\"pointerType\":\"mouse\"}";
	b.Cat(",\"actions\":["+md.ToString()+","+mu.ToString());
	b.Cat("]");
	b.Cat("}]}");
	http.Post(b);
	String content = http.Execute();
	if (isResponseOK()) {
		return getNoneValue(content);
	}
	if (http.GetStatusCode() == 501) {
	    http.Url(driverUrl+"/session/"+session+"/click");
	    Json elc;
	    elc("element",element)("button",button);
	    b = elc.ToString();
		http.Post(b);
		String content = http.Execute();
		return getNoneValue(content);
	}
	return false;
}
/*
**
** Double clicks the mouse on current position. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error. If the element is not interactable returns not interactable error.
** 
** @param element The id of web element
** @return true if http request success, false otherwise
**
** NBR-07-01-21
**
*/
bool driver::doubleClick(String element){
	String b;
	int button = 0;
	http.Url(driverUrl+"/session/"+session+"/actions");
	Json d,el,pt,mm,md,mu;
	el("ELEMENT",element)(lastElementHandle,element);
	mm("duration",100);
	md("type","pointerDown")("button",button);
	mu("type","pointerUp")("button",button);
	b = "{\"actions\":[{\"id\":\"default mouse\",\"type\":\"pointer\",\"parameters\":{\"pointerType\":\"mouse\"}";
	b.Cat(",\"actions\":["+md.ToString()+","+mu.ToString()+","+md.ToString()+","+mu.ToString());
	b.Cat("]");
	b.Cat("}]}");
	http.Post(b);
	String content = http.Execute();
	String value="";
	if (isResponseOK()) {
		return getStringValue(content,value);
	}
	if (http.GetStatusCode() == 501) {
	    http.Url(driverUrl+"/session/"+session+"/doubleclick");
	    Json elc;
	    elc("element",element)("button",button);
	    b = elc.ToString();
		http.Post(b);
		String content = http.Execute();
		return getStringValue(content,value);
	}
	return false;
}

/*
**
** Moves mouse to web element. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error. If the element is not interactable returns not interactable error.
** 
** @param element The id of web element
** @param x Offset of x-axis
** @param y Offset of y-axis
** @return true if http request success, false otherwise
**
** NBR-07-01-21
**
*/
bool driver::moveto(String element, int x, int y){
	String b = "";
	http.Url(driverUrl+"/session/"+session+"/actions");
	Json d,el,pt,mm,md,mu;
	el("ELEMENT",element)(lastElementHandle,element);
	mm("duration",100)("x",x+1)("y",y+1)("type","pointerMove")("origin",el);
	md("type","pointerDown")("button",0);
	mu("type","pointerUp")("button",0);
	b = "{\"actions\":[{\"id\":\"default mouse\",\"type\":\"pointer\",\"parameters\":{\"pointerType\":\"mouse\"}";
	b.Cat(",\"actions\":["+mm.ToString());
	//                           ","+md.ToString()+
	//                           ","+mu.ToString()
	b.Cat("]");
	b.Cat("}]}");
	http.Post(b);
	String content = http.Execute();
	if (isResponseOK()) {
		return getNoneValue(content);
	}

	if (http.GetStatusCode() == 501) {
		http.Url(driverUrl+"/session/"+session+"/moveto");
		Json elc;
		elc("element",element)("xoffset",x+1)("yoffset",y+1);
		b = elc.ToString();
		http.Post(b);
		String content = http.Execute();
		return getNoneValue(content);
	}
	return false;
}

/*
**
** Send mouse down command to input element. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error. If the element is not interactable returns not interactable error.
** 
** @param element The id of web element
** @param button 0-Left, 1-Middle, 2-Right button
** @return true if http request success, false otherwise
**
** NBR-07-01-21
**
*/
bool driver::mouseDown(String element, int button){
	String b;
	http.Url(driverUrl+"/session/"+session+"/actions");
	Json d,el,pt,mm,md,mu;
	el("ELEMENT",element)(lastElementHandle,element);
	mm("duration",100)("x",0)("y",0)("type","pointerMove")("origin",el);
	md("duration",100)("type","pointerDown")("button",button)("origin",el);
	mu("type","pointerUp")("button",0);
	b = "{\"actions\":[{\"id\":\"default mouse\",\"type\":\"pointer\",\"parameters\":{\"pointerType\":\"mouse\"}";
	b.Cat(",\"actions\":["+md.ToString());
	b.Cat("]");
	b.Cat("}]}");
	http.Post(b);
	String content = http.Execute();
	String value= "";
	if (isResponseOK()) {
		return getStringValue(content, value);
	}

	if (http.GetStatusCode() == 501) {
		http.Url(driverUrl+"/session/"+session+"/buttondown");
		Json elc;
		elc("element",element)("button",button);
		b = elc.ToString();
		http.Post(b);
		content = http.Execute();
		return getStringValue(content, value);
	}
	return false;
}

/*
**
** Send mouse up command to input element. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error. If the element is not interactable returns not interactable error.
** 
** @param element The id of web element
** @param button 0-Left, 1-Middle, 2-Right button
** @return true if http request success, false otherwise
**
** NBR-07-01-21
**
*/
bool driver::mouseUp(String element, int button){
	String b;
	http.Url(driverUrl+"/session/"+session+"/actions");
	Json d,el,pt,mm,md,mu;
	el("ELEMENT",element)(lastElementHandle,element);
	mm("duration",100)("x",0)("y",0)("type","pointerMove")("origin",el);
	md("type","pointerDown")("button",0);
	mu("duration",100)("type","pointerUp")("button",button)("origin",el);
	b = "{\"actions\":[{\"id\":\"default mouse\",\"type\":\"pointer\",\"parameters\":{\"pointerType\":\"mouse\"}";
	b.Cat(",\"actions\":["+mu.ToString());
	b.Cat("]");
	b.Cat("}]}");
	http.Post(b);
	String content = http.Execute();
	String value= "";
	if (isResponseOK()) {
		return getStringValue(content, value);
	}

	if (http.GetStatusCode() == 501) {
		http.Url(driverUrl+"/session/"+session+"/buttonup");
		Json elc;
		elc("element",element)("button",button);
		b = elc.ToString();
		http.Post(b);
		content = http.Execute();
		return getStringValue(content, value);
	}
	return false;
}

/*
**
** Clears input element's value. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error. If the element is not interactable returns not interactable error.
** 
** @param element The id of web element
** @return true if http request success, false otherwise
**
** NBR-06-01-21
**
*/
bool driver::clear(String element){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/clear");
	http.Post("{}");
	String content = http.Execute();
	return getNoneValue(content);
}

/*
**
** Sends a sequence of key strokes to web element. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error. 
** 
** @param element The id of web element
** @param text The text which will be typed
** @return true if http request success, false otherwise
**
** NBR-06-01-21
**
*/
bool driver::type(String element,String text){
	String keyCode;
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/value");
	keyCode = text;
	WString nullStr;
	nullStr.Cat({0xE000});
	bool addNull = false;
	RegExp r1("{(\\w+)}", RegExp::UNICODE);
	while(r1.GlobalMatch(text)){
		keyCode.Replace("{"+r1[0]+"}",keys.getKey("{"+r1[0]+"}"));
		addNull = (r1[0]=="NULL") ? false : true;
	}
	if (addNull) keyCode.Cat(nullStr.ToString());
	String b = "{\"text\":\""+keyCode+"\",\"value\":[\""+keyCode+"\"]}";
	http.Post(b);
	String content = http.Execute();
	return getNoneValue(content);
}
/*
**
** Retrieves current web page html source. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error. 
** 
** @param value Return page source
** @return true if http request success, false otherwise
**
** NBR-06-01-21
**
*/
bool driver::getPageSource(String &value){
	http.Url(driverUrl+"/session/"+session+"/source");
	http.GET();
	String content = http.Execute();
	return getStringValue(content, value);
}
/*
**
** Retrieves all cookis. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error. 
** 
** @param value Returns list of cookies as Json
** @return true if http request success, false otherwise
**
** NBR-05-01-21
**
*/
bool driver::getCookies(String &value){
	http.Url(driverUrl+"/session/"+session+"/cookie");
	http.GET();
	String content = http.Execute();
	return getStringValue(content, value);
}
/*
**
** Retrieves a cookie value. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error. 
** 
** @param name The name of cookie
** @param value Returns value of cookie
** @return true if http request success, false otherwise
**
** NBR-05-01-21
**
*/
bool driver::getCookie(String name,String &value){
	http.Url(driverUrl+"/session/"+session+"/cookie/"+name);
	http.GET();
	String content = http.Execute();
	return getStringValue(content, value);
}
/*
**
** Deletes all cookie. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error. 
** 
** @return true if http request success, false otherwise
**
** NBR-05-01-21
**
*/
bool driver::deleteCookies(){
	http.Url(driverUrl+"/session/"+session+"/cookie");
	http.DEL();
	String content = http.Execute();
	return getNoneValue(content);
}
/*
**
** Deletes a cookie. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error. 
** 
** @param name The name of cookie
** @return true if http request success, false otherwise
**
** NBR-05-01-21
**
*/
bool driver::deleteCookie(String name){
	http.Url(driverUrl+"/session/"+session+"/cookie/"+name);
	http.DEL();
	String content = http.Execute();
	return getNoneValue(content);
}

/*
**
** Adds a cookie. If current top-level browser is 
** no longer open, returns no such window error. If there is a browser alert,
** returns error. If web page is cookie-averse returns invalid cookie domain error.
** 
** @param domain The value if the entry exists, otherwise the current browsing context’s active document’s URL domain
** @param expiry The value if the entry exists, otherwise leave unset to indicate that this is a session cookie.
** @param httpOnly The value if the entry exists, otherwise false.
** @param name The name of cookie
** @param path The value if the entry exists, otherwise "/"
** @param secure The value if the entry exists, otherwise false.
** @param value The value of cookie
** @return true if http request success, false otherwise
**
** NBR-05-01-21
**
*/
bool driver::addCookie(String domain, int expiry, bool httpOnly, String name, String path, bool secure, String value){
	http.Url(driverUrl+"/session/"+session+"/cookie");
	Json Data,cookie;
	cookie("domain",domain)("expiry",expiry)("httpOnly",httpOnly)("name",name)("path",path)("secure",secure)("value",value);
	Data("cookie",cookie);
	http.Post(Data.ToString());
	String content = http.Execute();
	return getStringValue(content, value);
}

/*
**
** Accepts or Dismisses current alert. If current top-level browser is 
** no longer open, returns no such window error. If there is no current browser alert,
** returns no such alert.
** 
** @param type Alert command type. Values may be accept or dismiss 
** @return true if http request success, false otherwise
**
** NBR-04-01-21
**
*/
bool driver::alert(String type){
	http.Url(driverUrl+"/session/"+session+"/alert/"+type);
	http.Post("{}");
	String content = http.Execute();
	return getNoneValue(content);
}

/*
**
** Gets the text field of top-level browser alert. If current top-level browser is 
** no longer open, returns no such window error. If there is no current browser alert,
** returns no such alert.
** 
** @param text Returns value of entered alert text
** @return true if http request success, false otherwise
**
** NBR-04-01-21
**
*/
bool driver::getAlertText(String &value){
	http.Url(driverUrl+"/session/"+session+"/alert/text");
	http.GET();
	String content = http.Execute();
	return getStringValue(content, value);
}
/*
**
** Sets the text field of top-level browser alert. If current top-level browser is 
** no longer open, returns no such window error. If there is no current browser alert,
** returns no such alert.
** 
** @param text The value of alert text field
** @return true if http request success, false otherwise
**
** NBR-04-01-21
**
*/
bool driver::sendAlert(String text){
	http.Url(driverUrl+"/session/"+session+"/alert/text");
	Json Data;
	Data("text",text);
	http.Post(Data.ToString());
	String content = http.Execute();
	return getNoneValue(content);
}
/*
**
** Takes a screenshot of top-level browser viewport. If current top-level browser is 
** no longer open, returns no such window error.
** 
** @param value Returns element screenshot as base64 string
** @return true if http request success, false otherwise
**
** NBR-04-01-21
**
*/
bool driver::takeScreenShot(String &value){
	http.Url(driverUrl+"/session/"+session+"/screenshot");
	http.GET();
	String content = http.Execute();
	return getStringValue(content, value);
}
/*
**
** Takes a screenshot of the visible region of an element. If current top-level browser is 
** no longer open, returns no such window error.
** 
** @param element The Id of element
** @param value Returns element screenshot as base64 string
** @return true if http request success, false otherwise
**
** NBR-04-01-21
**
*/
bool driver::takeScreenShot(String element, String &value){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/screenshot");
	http.GET();
	String content = http.Execute();
	return getStringValue(content, value);
}

/*
**
** Checks http request response status.
** If response code is not 200 returns false
** 
** @return true if http request success, false otherwise
**
** NBR-12-30-20
**
*/
bool driver::isResponseOK(){
	if (http.GetError() != 0) {
		lastError = WEB_NO_DRIVER;
	} else {
		if (http.GetStatusCode() == 200){
			return true;
		}
		switch (http.GetStatusCode()) {
			case 0: lastError = WEB_TIMEOUT;break;
			case 500: lastError = WEB_NO_BROWSER;break;
			default : lastError = http.GetErrorDesc();break;
		}
	}
	return false;
}

/*
** Checks http request response status and return string value.
** 
** @param content response content eg({"value":"result"})
** @param value String value of request result
** @return true if there is no error status, false otherwise
**
** NBR-12-30-20
**
*/
bool driver::getStringValue(String content, String &value){
	if (!isResponseOK()) {
		return false;
	}
	if (http.GetStatusCode() == 200){
		Value b = ParseJSON(content);
		if ((b["status"] !="")&&(b["status"] != 0)) {
			Value err = b["value"];
			lastError = err["message"];
			return false;
		}
		value << b["value"];
		return true;
	}
	return false;
}
/*
** Checks http request response status and return string value.
** 
** @param content response content eg({"value":"result"})
** @param value Json String value of request result
** @return true if there is no error status, false otherwise
**
** NBR-12-30-20
**
*/
bool driver::getJsonValue(String name, String content, String &value){
	if (!isResponseOK()) {
		return false;
	}
	if (http.GetStatusCode() == 200){
		Value b = ParseJSON(content);
		if ((b["status"] !="")&&(b["status"] != 0)) {
			Value err = b["value"];
			lastError = err["message"];
			return false;
		}
		Json Data;
		Data(name,b["value"]);
		value = Data.ToString();
		return true;
	}
	return false;
}
/*
** Checks http request response status and return boolean value.
** 
** @param content response content eg({"value":true})
** @param value Boolean value of request result
** @return true if there is no error status, false otherwise
**
** NBR-12-30-20
**
*/
bool driver::getBooleanValue(String content, bool &value){
	if (!isResponseOK()) {
		return false;
	}
	if (http.GetStatusCode() == 200){
		Value b = ParseJSON(content);
		if ((b["status"] !="")&&(b["status"] != 0)) {
			Value err = b["value"];
			lastError = err["message"];
			return false;
		}
		value = b["value"];
		return true;
	}
	return false;
}
/*
** Checks http request response status and return no value.
** 
** @param content response content eg({"value":true})
** @return true if there is no error status, false otherwise
**
** NBR-12-30-20
**
*/
bool driver::getNoneValue(String content){
	if (!isResponseOK()) {
		return false;
	}
	if (http.GetStatusCode() == 200){
		Value b = ParseJSON(content);
		if ((b["status"] !="")&&(b["status"] != 0)) {
			Value err = b["value"];
			lastError = err["message"];
			return false;
		}
		return true;
	}
	return false;
}
/*
**
** Executes javascript on current browsing context. If current top-level browser is 
** no longer open, returns no such window error. If session script timeout miliseconds is
** reached returns script timeout error.
** 
** Doesnt support script arguments. 
**
** @param script The javascript code which will be executed
** @param value Return value of javascript
** @param async The boolean value of async. if Trus it is async else async
** @return true if http request ends, false otherwise
**
** NBR-12-30-20
**
*/
bool driver::executeScript(String script, String &value, bool async) {
	int timeout = 10000;
	getTimeout("script",timeout);
	http.RequestTimeout(timeout);
	http.Timeout(timeout);
	http.Url(driverUrl+"/session/"+session+"/execute/"+((async) ? "async" : "sync"));
	Json Data;
	ValueArray va;
	Data("script",script)("args",va);
	http.Post(Data.ToString());
	String content = http.Execute();
	return getStringValue(content, value);
}

/*
**
** Waits until page load.
**
** @param wait The boolean value to check page load
** @return true if document state is complete, false otherwise
**
** NBR-12-28-20
**
*/
bool driver::waitUntilPageLoad(bool wait) {
	if (wait) {
		String ret;
		if (executeScript("return document.readyState",ret)) {
			if (ret == "complete")
				return true;
		} else return false;
	}
	return true;
}

/*
**
** Waits until element visible.
**
** @param element, The given element
** @return true if document state is complete, false otherwise
**
** NBR-12-28-20
**
*/
bool driver::waitUntilVisible(String element) {
	bool waitvisible = true;
    String elId;
    bool status = false;
    int i=0;
    while ((waitvisible) && (i++<15) && (getError() != WEB_NO_BROWSER)) {
        if (findElement(element, elId)) {
	        isDisplayed(elId, status);
	        if (status) waitvisible = false;
        }
        Sleep(500);
    }
    return not waitvisible;
}
/*
**
** Switch session will select session and commands will be send to the browser
** session with given handle. If the session is null, returns invalid session error.
**
** @param newSession The handle of the session
** @return true if switches to session successfully, false otherwise
**
** NBR-12-28-20
**
*/
bool driver::useSession(String newSession) {
	if (newSession == "") {
		setError(WEB_INVALID_SESSION);
		return false;
	}
	session = newSession;
	return true;
}

/*
**
** Sets error message of driver
**
** @param error Error message
**
** NBR-12-28-20
**
*/
void driver::setError(String error) {
	lastError = error;
}
