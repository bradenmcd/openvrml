import vrml.*;
import vrml.field.*;
import vrml.node.*;

/**
 * This is test class used to test the various classes and methods
 * provided by the JSAI and implemented in OpenVRML.
 *
 * Any failures or problems found during the test will be reported 
 * to the console.
 *
 * This class is meant to be used only with the TestJSAI.wrl sample world.
 *
 * Here are the objects and methods currently tested with this class.
 *
 * Browser
 *   getCurrentFrameRate
 *   getCurrentSpeed
 *   getName
 *   getVersion
 *   getWorldURL
 *   setDescription
 *   toString
 *
 * ConstSFBool
 *   getValue
 *   toString
 *
 * Event
 *   getName
 *   getValue
 *
 * Script
 *   getBrowser
 *   getEventOut
 *   getField
 *
 * SFColor
 *   getBlue
 *   getGreen
 *   getRed
 *   setValue(float, float, float)
 *
 *
 *
 * Methods known not to be not working
 *
 * SFColor
 *   toString 
 *
 */
class TestJSAI extends Script {

  private SFColor activeColor;
  private SFColor inactiveColor;
  private SFColor color;

  public void initialize() {

    /* TEST Script Object */

    /* Test getField */
    activeColor = (SFColor) getField("activeColor");
// TODO: This call causes a segfault in SFColor::print
//    System.out.println("activeColor = " + activeColor);
    System.out.println("activeColor = " +
                       "[" + activeColor.getRed() + "][" +
                       activeColor.getGreen() + "][" + 
                       activeColor.getBlue() + "]");
    SFColor inactiveColor = (SFColor) getField("inactiveColor");
    Field invalidField = getField("blah2");
    if (invalidField != null) System.out.println("getField(1) failed");

    /* Test getEventOut */
    color = (SFColor) getEventOut("color");
    SFColor activeColorEventOut = (SFColor)getEventOut("activeColor");
    if (activeColorEventOut == null) 
      System.out.println("getEventOut(1) failed");
    Field invalidEventOut = getEventOut("blah");
    if (invalidEventOut != null)
      System.out.println("getEventOut(2)  failed");

    /* Test getEventIn */
    SFBool isActiveEventIn = (SFBool)getEventIn("isActive");
    if (isActiveEventIn == null) System.out.println("getEventIn 1 failed");
    SFColor activeColorEventIn = (SFColor)getEventIn("activeColor");
    if (activeColorEventIn == null) 
      System.out.println("getEventIn 2 failed");
    Field invalidEventIn = getEventIn("blahblah");
    if (invalidEventIn != null)
      System.out.println("getEventIn 3 failed");

    /* Test getBrowser */
    System.out.println("Print Browser Information");
    Browser browser = getBrowser();
    System.out.println(browser); // test Browser.toString()
    System.out.println("Name = " + browser.getName());
    System.out.println("Version = " + browser.getVersion());
    System.out.println("Current Speed = " + browser.getCurrentSpeed());
    System.out.println("Current Frame Rate = " + 
                       browser.getCurrentFrameRate());
    System.out.println("World URL = " + browser.getWorldURL());
    /* Set Browser Description */
    browser.setDescription("JSAI Test World");
    System.out.println("exit initialize method");
  }

  public void processEvent (Event event) 
  {
    try 
    {
      System.out.println("in process event in java");
      System.out.println("Event name = " + event.getName());
      if (event.getName().equals("isActive")) {
        System.out.println("Event name = isActive is true");
      ConstSFBool test = (ConstSFBool)event.getValue();
      System.out.println("test of ConstSFBool toString = " + test);
      float val1 = 0.5F;
      float val2 = 0.8F;

      if (test.getValue()){
        System.out.println("set to active color");
        color.setValue(val2, val2, val2);}
      else {
        System.out.println("set to inactive color");
        color.setValue(val1, val1, val1);}
      }
    }
    catch(Exception e)
    {
      e.printStackTrace();
    }
    System.out.println("exit processEvent in java");
  }
}
