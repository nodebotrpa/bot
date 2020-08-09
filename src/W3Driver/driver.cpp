#include "driver.hpp"
#include <plugin/pcre/Pcre.h>

bool driver::createSession(){
	http.Header("Content-Type","application/json");
	http.Url(driverUrl+"/session");
	http.Post(cap.getJson());
	String content = http.Execute();
	String dirs;
	if (http.GetError() != 0) {
		char dir[512];
		GetCurrentDirectory(512,dir);//GetModuleFileName
		dirs<<dir;
		dirs.Cat("\\driver");
		ShellExecute(NULL,"open","chromedriver.exe",NULL,dirs,SW_HIDE);
		content = http.Execute();
	}
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		MessageBox(NULL,"Can not start chromedriver.exe at "+dirs,"Error",MB_ICONERROR);
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
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}

bool driver::deleteSession(){
	http.Url(driverUrl+"/session/"+session);
	http.DEL();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"DeleteSession:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}

bool driver::navigateTo(String url){
	http.Url(driverUrl+"/session/"+session+"/url");
	Json Data;
	Data("url",url);
	http.Post(Data.ToString());
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"NavigateTo:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] !="") && (b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
		}
		else {
			if (http.GetStatusCode() == 500) {
				lastError = "PageLoad timeout may be too short. Change timeout properties at new session node or check chromedriver log file for error description";
				return false;
			} else {
				lastError = http.GetErrorDesc();
				return false;
			}
		}
	}
	return true;
}
bool driver::navigate(String direction){
	http.Url(driverUrl+"/session/"+session+"/"+direction);
	http.Post("{}");
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			Value b = ParseJSON(content);
			//LOG"Navigate:"+content+"\n");
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::getAllSessions(String &handle){
	http.Url(driverUrl+"/sessions");
	http.Get();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			Value b = ParseJSON(content);
			//LOG"All Sessions:"+content+"\n");
			if ((b["status"] !="")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
			Json Data;
			Data("sessions",b["value"]);
			handle = Data.ToString();
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::getTitle(String &title){
	http.Url(driverUrl+"/session/"+session+"/title");
	http.GET();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			Value b = ParseJSON(content);
			//LOG"GetTitle:"+content+"\n");
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
			title = b["value"];
		}
		else {
			Cout()<<http.GetStatusCode();
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::getUrl(String &url){
	http.Url(driverUrl+"/session/"+session+"/url");
	http.GET();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			Value b = ParseJSON(content);
			//LOG"GetUrl:"+content+"\n");
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				if (b["status"]==100) lastError = "Browser is closed";
				return false;
			}
			url = b["value"];
		}
		else {
			if ((http.GetStatusCode() == 500) || (http.GetStatusCode() == 0))
				lastError = "Browser is closed";
			 else
			    lastError = http.GetErrorDesc();
			Cout()<<http.GetStatusCode();
			return false;
		}
	}
	return true;
}
/*
** timeout : implicit, script, pageLoad
*/
bool driver::setTimeout(String timeout, int seconds){
	http.Url(driverUrl+"/session/"+session+"/timeouts");
	if (seconds<100) seconds = seconds*1000;
	Json Data;
	Data(timeout,seconds);
	http.Post(Data.ToString());
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"SetTimeout:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;

}
bool driver::getWindowHandle(String &handle){
	http.Url(driverUrl+"/session/"+session+"/window");
	http.GET();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"GetWindowHandle:"+content+"\n");
			Value b = ParseJSON(content);
			if (b["status"] != "") {
				if (b["status"] != 0) {
					Value err = b["value"];
					lastError = err["message"];
					return false;
				}
			}
			handle = b["value"];
		}
		else {
			Cout()<<http.GetStatusCode();
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::closeWindow(){
	http.Url(driverUrl+"/session/"+session+"/window");
	http.DEL();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"CloseWindow:"+content+"\n");
			Value b = ParseJSON(content);
			if (b["status"]!="") {
				if (b["status"] != 0) {
					Value err = b["value"];
					lastError = err["message"];
					return false;
				}
			}
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::switchToWindow(String window){
	http.Url(driverUrl+"/session/"+session+"/window");
	Json Data;
	Data("handle",window)("name",window);
	http.Post(Data.ToString());
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"SwitchToWindow:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::getWindowHandles(String &handles){
	http.Url(driverUrl+"/session/"+session+"/window/handles");
	http.GET();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"GetWindowHandles:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
			Json Data;
			Data("handles",b["value"]);
			handles = Data.ToString();
		}
		else {
			Cout()<<http.GetStatusCode();
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::switchToFrame(String frame){
	http.Url(driverUrl+"/session/"+session+"/frame");
	Json Data, Element;
	Element("ELEMENT",frame)(lastElementHandle,frame);
	Data("id",Element);
	if (frame == "null") http.Post("{ \"id\" : null }");
	else http.Post(Data.ToString());
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"SwitchToFrame:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::switchToParentFrame(){
	http.Url(driverUrl+"/session/"+session+"/frame/parent");
	http.Post("{}");
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"SwitchToParentFrame:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::getWindowRect(String &rect){
	http.Url(driverUrl+"/session/"+session+"/window/rect");
	http.GET();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"GetWindowRect:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
			Json Data;
			Data("rect",b["value"]);
			rect = Data.ToString();
		}
		else {
			Cout()<<http.GetStatusCode();
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::setWindowRect(int x, int y, int width, int height){
	http.Url(driverUrl+"/session/"+session+"/window/rect");
	Json Data;
	Data("x",x)("y",y)("width",width)("height",height);
	http.Post(Data.ToString());
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"SetWindowRect:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
		}
		else {
			Cout()<<http.GetStatusCode();
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::minimize(){
	http.Url(driverUrl+"/session/"+session+"/window/minimize");
	http.Post("{}");
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"Minimize:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
		}
		else {
			Cout()<<http.GetStatusCode();
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::maximize(){
	http.Url(driverUrl+"/session/"+session+"/window/maximize"); // window/current/maximize
	http.Post("{}");
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"Maximize:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
		}
		else {
			Cout()<<http.GetStatusCode();
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::fullScreen(){
	http.Url(driverUrl+"/session/"+session+"/window/fullscreen");
	http.Post("{}");
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"FullScreen:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
		}
		else {
			Cout()<<http.GetStatusCode();
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::findElement(String xpath, String &handle){
	http.Url(driverUrl+"/session/"+session+"/element");
	Json Data;
	Data("using","xpath")("value",xpath);
	http.Post(Data.ToString());
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"FindElement:"+content+"\n");
			Value b = ParseJSON(content);
			if (b["status"] != "") {
				if (b["status"] != 0) {
					Value err = b["value"];
					lastError = err["message"];
					return false;
				}
				Value el = b["value"];
				handle = el["ELEMENT"];
			} else {
				ValueMap el = b["value"];
				handle = el.GetValue(0);
				lastElementHandle = el.GetKey(0);
			}
		}
		else {
			if (http.GetStatusCode() == 404) {
				lastError = "Unable to locate element : "+xpath;
				return false;
			}
			if (http.GetStatusCode() == 500) {
				lastError = "Chrome browser is not running";
			}else {
				lastError = http.GetErrorDesc();
				return false;
			}
		}
	}
	return true;
}
bool driver::findElements(String xpath, String &handle){
	http.Url(driverUrl+"/session/"+session+"/elements");
	Json Data;
	Data("using","xpath")("value",xpath);
	http.Post(Data.ToString());
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"FindElements:"+content+"\n");
			Value b = ParseJSON(content);
			if (b["status"] != "") {
				if (b["status"] != 0) {
					Value err = b["value"];
					lastError = err["message"];
					return false;
				}
			}
			Json Data;
			Data("elements",b["value"]);
			handle = Data.ToString();
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::findElement(String element, String xpath, String &handle){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/element");
	Json Data;
	Data("using","xpath")("value",xpath);
	http.Post(Data.ToString());
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"FindElementFromElement:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
			Value el = b["value"];
			handle = el["ELEMENT"];
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::findElements(String element, String xpath, String &handle){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/elements");
	Json Data;
	Data("using","xpath")("value",xpath);
	http.Post(Data.ToString());
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"FindElementsFromElement:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
			Json Data;
			Data("elements",b["value"]);
			handle = Data.ToString();
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::getActiveElement(String &handle){
	http.Url(driverUrl+"/session/"+session+"/element/active");
	http.GET();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"GetActiveElement:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
			Value el = b["value"];
			handle = el["ELEMENT"];
		}
		else {
			Cout()<<http.GetStatusCode();
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::isSelected(String element, bool &status){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/selected");
	http.GET();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			Value b = ParseJSON(content);
			//LOG"Is Selected:"+content+"\n");
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
			status = b["value"];
		}
		else {
			Cout()<<http.GetStatusCode();
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::isDisplayed(String element, bool &status){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/displayed");
	http.GET();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			Value b = ParseJSON(content);
			//LOG"Is Displayed:"+content+"\n");
			if (b["status"] != "") {
				if (b["status"] != 0) {
					Value err = b["value"];
					lastError = err["message"];
					return false;
				}
			}
			status = b["value"];
		}
		else {
			Cout()<<http.GetStatusCode();
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::getElementAttribute(String element, String name, String &value){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/attribute/"+name);
	http.GET();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			Value b = ParseJSON(content);
			//LOG"GetElementAttribute:"+content+"\n");
			if (b["status"] != "") {
				if (b["status"] != 0) {
					Value err = b["value"];
					lastError = err["message"];
					return false;
				}
			}
			value = b["value"];
		}
		else {
			Cout()<<http.GetStatusCode();
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::getElementProperty(String element, String name, String &value){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/property/"+name);
	http.GET();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			Value b = ParseJSON(content);
			//LOG"GetElementProperty:"+content+"\n");
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
			value = b["value"];
		}
		else {
			Cout()<<http.GetStatusCode();
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::getElementCss(String element, String name, String &value){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/css/"+name);
	http.GET();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			Value b = ParseJSON(content);
			//LOG"GetElementCss:"+content+"\n");
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
			value = b["value"];
		}
		else {
			Cout()<<http.GetStatusCode();
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::getElementText(String element, String &value){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/text");
	http.GET();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			Value b = ParseJSON(content);
			//LOG"GetElementText:"+content+"\n");
			if (b["status"] != "") {
				if (b["status"] != 0) {
					Value err = b["value"];
					lastError = err["message"];
					return false;
				}
			}
			value = b["value"];
		}
		else {
			Cout()<<http.GetStatusCode();
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::getElementTag(String element, String &value){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/name");
	http.GET();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			Value b = ParseJSON(content);
			//LOG"GetElementTag:"+content+"\n");
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
			value = b["value"];
		}
		else {
			Cout()<<http.GetStatusCode();
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::getElementRect(String element, String &value){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/rect");
	http.GET();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			Value b = ParseJSON(content);
			//LOG"GetElementRect:"+content+"\n");
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
			Json Data;
			Data("rect",b["value"]);
			value = Data.ToString();
		}
		else {
			Cout()<<http.GetStatusCode();
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::isEnabled(String element, bool &status){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/enabled");
	http.GET();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			Value b = ParseJSON(content);
			//LOG"IsEnabled:"+content+"\n");
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
			status = b["value"];
		}
		else {
			Cout()<<http.GetStatusCode();
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::moveto(String element, int x, int y){
	String b = "";
	if (browser == "CHROME"){
		http.Url(driverUrl+"/session/"+session+"/moveto");
		Json elc;
		elc("element",element)("xoffset",x+1)("yoffset",y+1);
		b = elc.ToString();
	}
	http.Post(b);
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"Hover:"+content+"\n");
			Value b = ParseJSON(content);
			if (b["status"] != "") {
				if (b["status"] != 0) {
					Value err = b["value"];
					lastError = err["message"];
					return false;
				}
			}
		}
		else {
			if (http.GetStatusCode() == 404) {
				http.Url(driverUrl+"/session/"+session+"/actions");
				Json d,el,pt,mm,md,mu;
				el("ELEMENT",element)(lastElementHandle,element);
				mm("duration",100)("x",x+1)("y",y+1)("type","pointerMove")("origin",el);
				md("type","pointerDown")("button",0);
				mu("type","pointerUp")("button",0);
				String b = "{\"actions\":[{\"id\":\"default mouse\",\"type\":\"pointer\",\"parameters\":{\"pointerType\":\"mouse\"}";
				b.Cat(",\"actions\":["+mm.ToString());
				//                           ","+md.ToString()+
				//                           ","+mu.ToString()
				b.Cat("]");
				b.Cat("}]}");
				http.Post(b);
				String content = http.Execute();
				if (http.GetError() != 0) {
					lastError = "Driver is not running";
					return false;
				}
				else {
					if (http.GetStatusCode() == 200){
						//LOG"Hover:"+content+"\n");
						Value b = ParseJSON(content);
						if (b["status"] != "") {
							if (b["status"] != 0) {
								Value err = b["value"];
								lastError = err["message"];
								return false;
							}
						}
					} else {
						lastError = http.GetErrorDesc();
						return false;
					}
				}
			} else {
				lastError = http.GetErrorDesc();
				return false;
			}
		}
	}
	return true;
}
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
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"Click:"+content+"\n");
			Value b = ParseJSON(content);
			if (b["status"] != "") {
				if (b["status"] != 0) {
					Value err = b["value"];
					lastError = err["message"];
					return false;
				}
			}
		}
		else {
			if (http.GetStatusCode() == 501) {
			    http.Url(driverUrl+"/session/"+session+"/click");
			    Json elc;
			    elc("element",element)("button",button);
			    b = elc.ToString();
				http.Post(b);
				String content = http.Execute();
				if (http.GetError() != 0) {
					lastError = "Driver is not running";
					return false;
				}
				else {
					if (http.GetStatusCode() == 200){
						//LOG"Hover:"+content+"\n");
						Value b = ParseJSON(content);
						if ((b["status"] != "")&&(b["status"] != 0)) {
							Value err = b["value"];
							lastError = err["message"];
							return false;
						}
					} else {
						lastError = http.GetErrorDesc();
						return false;
					}
				}
			} else {
				lastError = http.GetErrorDesc();
				return false;
			}
		}
	}
	return true;
}
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
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"Click:"+content+"\n");
			Value b = ParseJSON(content);
			if (b["status"] != "") {
				if (b["status"] != 0) {
					Value err = b["value"];
					lastError = err["message"];
					return false;
				}
			}
		}
		else {
			if (http.GetStatusCode() == 501) {
			    http.Url(driverUrl+"/session/"+session+"/doubleclick");
			    Json elc;
			    elc("element",element)("button",button);
			    b = elc.ToString();
				http.Post(b);
				String content = http.Execute();
				if (http.GetError() != 0) {
					lastError = "Driver is not running";
					return false;
				}
				else {
					if (http.GetStatusCode() == 200){
						//LOG"Hover:"+content+"\n");
						Value b = ParseJSON(content);
						if ((b["status"] != "")&&(b["status"] != 0)) {
							Value err = b["value"];
							lastError = err["message"];
							return false;
						}
					} else {
						lastError = http.GetErrorDesc();
						return false;
					}
				}
			} else {
				lastError = http.GetErrorDesc();
				return false;
			}
		}
	}
	return true;
}
bool driver::mouseDown(String element, int button){
	String b;
	if (browser == "CHROME"){
		http.Url(driverUrl+"/session/"+session+"/buttondown");
		Json elc;
		elc("element",element)("button",button);
		b = elc.ToString();
	}
	http.Post(b);
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"MouseDown:"+content+"\n");
			Value b = ParseJSON(content);
			if (b["status"] != "") {
				if (b["status"] != 0) {
					Value err = b["value"];
					lastError = err["message"];
					return false;
				}
			}
		}
		else {
			if (http.GetStatusCode() == 404) {
				http.Url(driverUrl+"/session/"+session+"/actions");
				Json d,el,pt,mm,md,mu;
				el("ELEMENT",element)(lastElementHandle,element);
				mm("duration",100)("x",0)("y",0)("type","pointerMove")("origin",el);
				md("duration",100)("type","pointerDown")("button",button)("origin",el);
				mu("type","pointerUp")("button",0);
				String b = "{\"actions\":[{\"id\":\"default mouse\",\"type\":\"pointer\",\"parameters\":{\"pointerType\":\"mouse\"}";
				b.Cat(",\"actions\":["+md.ToString());
				b.Cat("]");
				b.Cat("}]}");
				http.Post(b);
				String content = http.Execute();
				if (http.GetError() != 0) {
					lastError = "Driver is not running";
					return false;
				}
				else {
					if (http.GetStatusCode() == 200){
						//LOG"Down:"+content+"\n");
						Value b = ParseJSON(content);
						if (b["status"] != "") {
							if (b["status"] != 0) {
								Value err = b["value"];
								lastError = err["message"];
								return false;
							}
						}
					} else {
						lastError = http.GetErrorDesc();
						return false;
					}
				}
			} else {
				lastError = http.GetErrorDesc();
				return false;
			}
		}
	}
	return true;
}
bool driver::mouseUp(String element, int button){
	String b;
	if (browser == "CHROME"){
		http.Url(driverUrl+"/session/"+session+"/buttonup");
		Json elc;
		elc("element",element)("button",button);
		b = elc.ToString();
	}
	http.Post(b);
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"MouseUp:"+content+"\n");
			Value b = ParseJSON(content);
			if (b["status"] != "") {
				if (b["status"] != 0) {
					Value err = b["value"];
					lastError = err["message"];
					return false;
				}
			}
		}
		else {
			if (http.GetStatusCode() == 404) {
				http.Url(driverUrl+"/session/"+session+"/actions");
				Json d,el,pt,mm,md,mu;
				el("ELEMENT",element)(lastElementHandle,element);
				mm("duration",100)("x",0)("y",0)("type","pointerMove")("origin",el);
				md("type","pointerDown")("button",0);
				mu("duration",100)("type","pointerUp")("button",button)("origin",el);
				String b = "{\"actions\":[{\"id\":\"default mouse\",\"type\":\"pointer\",\"parameters\":{\"pointerType\":\"mouse\"}";
				b.Cat(",\"actions\":["+mu.ToString());
				b.Cat("]");
				b.Cat("}]}");
				http.Post(b);
				String content = http.Execute();
				if (http.GetError() != 0) {
					lastError = "Driver is not running";
					return false;
				}
				else {
					if (http.GetStatusCode() == 200){
						//LOG"Down:"+content+"\n");
						Value b = ParseJSON(content);
						if (b["status"] != "") {
							if (b["status"] != 0) {
								Value err = b["value"];
								lastError = err["message"];
								return false;
							}
						}
					} else {
						lastError = http.GetErrorDesc();
						return false;
					}
				}
			} else {
				lastError = http.GetErrorDesc();
				return false;
			}
		}
	}
	return true;
}
bool driver::clear(String element){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/clear");
	http.Post("{}");
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"ElementClear:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::type(String element,String text){
	String keyCode;
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/value");
	keyCode = text;
	/*WString ctrlStr;
	ctrlStr.Cat({0xE009});
	*/
	WString nullStr;
	nullStr.Cat({0xE009});
	bool addNull = false;
	RegExp r1("{(\\w+)}", RegExp::UNICODE);
	while(r1.GlobalMatch(text)){
		keyCode.Replace("{"+r1[0]+"}",keys.getKey("{"+r1[0]+"}"));
		addNull = (r1[0]=="NULL") ? false : true;
	}
	if (addNull) keyCode.Cat(nullStr.ToString());
	String b = "{\"text\":\""+keyCode+"\",\"value\":[\""+keyCode+"\"]}";
	http.Post(b);
	Cout()<<b;
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"TypeText:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] !="")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::getPageSource(String element, String &value){
	http.Url(driverUrl+"/session/"+session+"/source");
	http.GET();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			Value b = ParseJSON(content);
			//LOG"GetPageSource:"+content+"\n");
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
			value = b["value"];
		}
		else {
			Cout()<<http.GetStatusCode();
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::getCookies(String &value){
	http.Url(driverUrl+"/session/"+session+"/cookie");
	http.GET();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"GetCookies:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
			Json Data;
			Data("cookies",b["value"]);
			value = Data.ToString();
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::getCookie(String name,String &value){
	http.Url(driverUrl+"/session/"+session+"/cookie/"+name);
	http.GET();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"GetCookie:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
			
			Json Data;
			Data("cookie",b["value"]);
			value = Data.ToString();
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::deleteCookies(){
	http.Url(driverUrl+"/session/"+session+"/cookie");
	http.DEL();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"DeleteCookies:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::deleteCookie(String name){
	http.Url(driverUrl+"/session/"+session+"/cookie/"+name);
	http.DEL();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"DeleteCookie:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::addCookie(String domain, int expiry, bool httpOnly, String name, String path, bool secure, String value){
	http.Url(driverUrl+"/session/"+session+"/cookie");
	Json Data,cookie;
	cookie("domain",domain)("expiry",expiry)("httpOnly",httpOnly)("name",name)("path",path)("secure",secure)("value",value);
	Data("cookie",cookie);
	http.Post(Data.ToString());
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"addCookie:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::alert(String type){
	http.Url(driverUrl+"/session/"+session+"/alert/"+type);
	http.Post("{}");
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"Alert:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}

bool driver::getAlertText(String &value){
	http.Url(driverUrl+"/session/"+session+"/alert/text");
	http.GET();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"GetAlertText:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
			value = b["value"];
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::sendAlert(String text){
	http.Url(driverUrl+"/session/"+session+"/alert/text");
	Json Data;
	Data("text",text);
	http.Post(Data.ToString());
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"SendKeysToAlert:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] != "")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::takeScreenShot(String &value){
	http.Url(driverUrl+"/session/"+session+"/screenshot");
	http.GET();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"TakeScreenShot:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] !="")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
			value = b["value"];
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}
bool driver::takeScreenShot(String element, String &value){
	http.Url(driverUrl+"/session/"+session+"/element/"+element+"/screenshot");
	http.GET();
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		if (http.GetStatusCode() == 200){
			//LOG"TakeScreenShotEl:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] !="")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
			value = b["value"];
		}
		else {
			lastError = http.GetErrorDesc();
			return false;
		}
	}
	return true;
}

bool driver::executeScript(String script, String &value, bool async) {
	http.RequestTimeout(10000);
	http.Timeout(10000);
	http.Url(driverUrl+"/session/"+session+"/execute/"+((async) ? "async" : "sync"));
	Json Data;
	ValueArray va;
	Data("script",script)("args",va);
	http.Post(Data.ToString());
	String content = http.Execute();
	if (http.GetError() != 0) {
		lastError = "Driver is not running";
		return false;
	}
	else {
		//http.GetStatusCode() == 0 means timeout
		if (http.GetStatusCode() == 200){
			//LOG"Execute script:"+content+"\n");
			Value b = ParseJSON(content);
			if ((b["status"] !="")&&(b["status"] != 0)) {
				Value err = b["value"];
				lastError = err["message"];
				return false;
			}
			value << b["value"];
		}
		else {
			lastError = http.GetErrorDesc();
			//LOGlastError);
			return false;
		}
	}
	return true;
}

bool driver::waitUntilPageLoad(bool wait) {
	if (wait) {
		String ret;
		if (executeScript("return document.readyState",ret)) {
			if (ret == "complete") wait = false;
		} else return false;
	}
	return true;
}

bool driver::useSession(String newSession) {
	if (newSession == "") {setError("Invalid session name"); return false;}
	session = newSession;
	return true;
}

void driver::setError(String err) {
	lastError = err;
}
