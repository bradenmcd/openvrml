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
 * Fully tested classes
 *   ConstSFBool
 *   ConstSFColor
 *   ConstSFFloat
 *   ConstSFInt32
 *   ConstSFString
 *   Event
 *   SFBool
 *   SFColor
 *   SFFloat
 *   SFInt32
 *   SFString
 *
 * Partially tested classes with list of tested methods.
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
 * ConstMFColor
 *   ConstMFColor(float[][])
 *   getSize
 *
 * Script
 *   getBrowser
 *   getEventOut
 *   getField
 *
 * Methods known not to be not working
 *
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
    SFColor inactiveColor = (SFColor) getField("inactiveColor");
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

    /* Test ConstSFBool/SFBool methods */
    SFBool testBool = (SFBool) getField("testBool");
    if (testBool == null)
      System.out.println("get SFBool field failed");
    else
      System.out.println("testBool = " + testBool);
    System.out.println("testBool = " + testBool.getValue());
    testBool.setValue(false);
    System.out.println("testBool = " + testBool.getValue());
    ConstSFBool testConstSFBool = new ConstSFBool(true);
    System.out.println("testConstSFBool = " + testConstSFBool);
    System.out.println("testConstSFBool = " + testConstSFBool.getValue());
    testBool.setValue(testConstSFBool);
    SFBool testSFBool = new SFBool(false);
    testBool.setValue(testSFBool);
    SFBool testSFBool2 = new SFBool();
    testSFBool2.setValue(true);
    testBool.setValue(testSFBool2);
    System.out.println("testBool = " + testBool);

    /* Test ConstSFColor/SFColor methods */
    System.out.println("activeColor = " + activeColor);
    System.out.println("activeColor = " +
                       "[" + activeColor.getRed() + "][" +
                       activeColor.getGreen() + "][" + 
                       activeColor.getBlue() + "]");
    ConstSFColor testConstSFColor = new ConstSFColor(1.0f, 0.0f, 1.0f);
    float[] colorArray = new float[3];
    testConstSFColor.getValue(colorArray);
    System.out.println("ConstSFColor.getValue = [" + colorArray[0] +
                       "][" + colorArray[1] + "][" + colorArray[2] + "]");
    System.out.println("ConstSFColor = [" + testConstSFColor.getRed() +
                       "][" + testConstSFColor.getGreen() + "][" +
                       testConstSFColor.getBlue() + "]");
    System.out.println("ConstSFColor = " + testConstSFColor);
    SFColor testDefaultSFColor = new SFColor();
    testDefaultSFColor.setValue(1.0f, 0.0f, 0.0f);
    testDefaultSFColor.setValue(testConstSFColor);
    SFColor testSFColor = new SFColor(1.0f, 1.0f, 0.0f);
    testSFColor.getValue(colorArray);
    System.out.println("SFColor.getValue = [" + colorArray[0] + "][" + 
                       colorArray[1] + "][" + colorArray[2] + "]");
    testDefaultSFColor.setValue(testSFColor);

    /* Test ConstSFFloat/SFFloat methods */
    SFFloat testFloat = (SFFloat) getField("testFloat");
    if (testFloat == null)
      System.out.println("get SFFloat field failed");
    else
      System.out.println("testFloat = " + testFloat);
    System.out.println("testFloat = " + testFloat.getValue());
    testFloat.setValue(10.0f);
    ConstSFFloat testConstSFFloat = new ConstSFFloat(2.0f);
    System.out.println("testConstFloat = " + testConstSFFloat);
    System.out.println("testConstFloat = " + testConstSFFloat.getValue());
    testFloat.setValue(testConstSFFloat);
    SFFloat testSFFloat = new SFFloat();
    SFFloat testSFFloat2 = new SFFloat(7.0f);
    testFloat.setValue(testSFFloat);

    /* Test ConstSFImage/SFImage methods */

    /* Test ConstSFInt32/SFInt32 methods */
    SFInt32 testInt32 = (SFInt32) getField("testInt32");
    if (testInt32 == null) 
      System.out.println("get SFInt32 field failed");
    else
      System.out.println("testInt32 = " + testInt32);
    System.out.println("testInt32 = " + testInt32.getValue());
    SFInt32 testSFInt32 = new SFInt32();
    SFInt32 testSFInt32_2 = new SFInt32(500);
    ConstSFInt32 testConstSFInt32 = new ConstSFInt32(100);
    System.out.println("testConstSFInt32 = " + testConstSFInt32);
    System.out.println("testConstSFInt32 = " + testConstSFInt32.getValue());
    testInt32.setValue(250);
    testInt32.setValue(testSFInt32);
    testInt32.setValue(testConstSFInt32);

    /* Test ConstSFNode/SFNode methods */

    /* Test ConstSFRotation/SFRotation methods */

    /* Test ConstSFString/SFString methods */
    SFString testString = (SFString) getField("testString");
    testString.setValue("testString2");
    System.out.println("testString = " + testString);
    Field invalidField = getField("blah2");
    if (invalidField != null) System.out.println("getField(1) failed");
    SFString testSFString1 = new SFString();
    SFString testSFString2 = new SFString("testing");
    testString.setValue(testSFString1);
    System.out.println("testSFString = " + testString.getValue());
    ConstSFString testConstSFString = new ConstSFString("ConstSFString");
    System.out.println("testConstSFString = " + testConstSFString);
    System.out.println("testConstSFString = " + testConstSFString.getValue());
    testString.setValue(testConstSFString);

    /* Test ConstSFTime/SFTime methods */

    /* Test ConstSFVec2f/SFVec2f methods */

    /* Test ConstSFVec3f/SFVec3f methods */

    /* Test ConstMFColor/MFColor methods */
    float[] colorArray1 = {1.0f, 0.0f, 1.0f};
    float[] colorArray2 = {0.0f, 0.0f, 1.0f};
    float[] colorArray3 = {1.0f, 1.0f, 0.0f};
    float[][] colorArrays = {colorArray1, colorArray2, colorArray3};
    ConstMFColor testConstMFColor1= new ConstMFColor(colorArrays);
    if (testConstMFColor1.getSize() != 3)
      System.out.println("ConstMFColor.getSize() failed");


    /* Test ConstMFFloat/MFFloat methods */

    /* Test ConstMFInt32/MFInt32 methods */

    /* Test ConstMFNode/MFNode methods */

    /* Test ConstMFRotation/MFRotation methods */

    /* Test ConstMFString/MFString methods */

    /* Test ConstMFTime/MFTime methods */

    /* Test ConstMFVec2f/MFVec2f methods */

    /* Test ConstMFVec3f/MFVec3f methods */
 
    System.out.println("exit initialize method");
  }

  public void processEvent (Event event) 
  {
    try 
    {
      System.out.println("in process event in java");
      /* Test Event class methods */
      System.out.println("Event name = " + event.getName());
      System.out.println("Event.toString = " + event);
      System.out.println("Event timestamp = " + event.getTimeStamp());
      Event clonedEvent = (Event)event.clone();
      if (!event.getName().equals(clonedEvent.getName()))
        System.out.println("Cloned event name does not equal event name");
      if (event.getTimeStamp() != clonedEvent.getTimeStamp())
        System.out.println("Cloned timestamp does not equal timestamp");

      if (event.getName() == "isActive")
      {
        ConstSFBool test = (ConstSFBool)event.getValue();
        System.out.println("test of ConstSFBool toString = " + test);
        float val1 = 0.5F;
        float val2 = 0.8F;

        if (test.getValue())
        {
          System.out.println("set to active color");
          color.setValue(val2, val2, val2);
        }
        else 
        {
          System.out.println("set to inactive color");
          color.setValue(val1, val1, val1);
        }
      }
    }
    catch(Exception e)
    {
      e.printStackTrace();
    }
    System.out.println("exit processEvent in java");
  }

  public void eventsProcessed()
  {
    System.out.println("enter eventsProcessed in java");
    System.out.println("exit eventsProcessed in java");
  }

  public void shutdown()
  {
    System.out.println("enter shutdown in java");
    System.out.println("exit shutdown in java");
  }
}
