#include <Core/Core.h>
#include "driver.hpp"

using namespace Upp;

CONSOLE_APP_MAIN
{
	driver brw;
	brw.setBrowser("CHROME");
	brw.createSession();
	brw.navigateTo("http://www.google.com");
	brw.waitUntilPageLoad();
	String searchField;
	brw.findElement("//input[@name='q']",searchField);
	brw.type(searchField,"ultimatepp");
	brw.type(searchField,"{ENTER}");
	Sleep(10000);
	brw.closeWindow();
}
