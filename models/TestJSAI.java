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
 *   ConstMFColor
 *   ConstMFFloat
 *   ConstMFInt32
 *   ConstMFString
 *   ConstSFBool
 *   ConstSFColor
 *   ConstSFFloat
 *   ConstSFImage
 *   ConstSFInt32
 *   ConstSFNode
 *   ConstSFRotation
 *   ConstSFString
 *   ConstSFTime
 *   ConstSFVec2f
 *   ConstSFVec3f
 *   Event
 *   MFColor
 *   MFFloat
 *   SFBool
 *   SFColor
 *   SFFloat
 *   SFImage
 *   SFInt32
 *   SFRotation
 *   SFString
 *   SFTime
 *   SFVec2f
 *   SFVec3f
 *
 * Partially tested classes with list of tested methods.
 *
 * BaseNode
 *   getBrowser
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
 * Node
 *   toString
 *
 * Script
 *   getBrowser
 *   getEventIn
 *   getEventOut
 *   getField
 *
 *
 * Methods known to be not working
 *
 * BaseNode
 *   getType (not implemented yet)
 *
 *
 * Classes not tested
 *   ConstMFNode
 *   ConstMFRotation
 *   ConstMFTime
 *   ConstMFVec2f
 *   ConstMFVec3f
 *   MFInt32
 *   MFNode
 *   MFRotation
 *   MFString
 *   MFTime
 *   MFVec2f
 *   MFVec3f
 *   SFNode
 */
class TestJSAI extends Script {

  private SFColor activeColor;
  private SFColor inactiveColor;
  private SFColor color;

  public void initialize()
  {
   try{
    boolean caught = false;

    /* TEST Script Object */

    /* Test getField */
    activeColor = (SFColor) getField("activeColor");
    SFColor inactiveColor = (SFColor) getField("inactiveColor");

    /* Test getEventOut */
    try
    {
      color = (SFColor) getEventOut("color");
    }
    catch (InvalidEventOutException exc)
    {
      caught = true;
    }

    if (caught)
      System.out.println("getEventOut test failed");

    caught = false;

    try
    {
      Field invalidEventOut = getEventOut("blah");
    }
    catch (InvalidEventOutException exc)
    {
      caught = true;
    }

    if (!caught)
      System.out.println("getEventOut test failed");
    else
      caught = false;

    /* Test getEventIn */
    SFBool isActiveEventIn = (SFBool)getEventIn("isActive");

    try
    {
      Field invalidEventIn = getEventIn("blahblah");
    }
    catch (InvalidEventInException exc)
    {
      caught = true;
    }

    if (!caught)
      System.out.println("getEventIn test failed");
    else
      caught = false;

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
    System.out.println("Test ConstSFBool/SFBool");
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
    System.out.println("Test ConstSFColor/SFColor");
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
    System.out.println("Test ConstSFFloat/SFFloat");
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
    System.out.println("Test ConstSFImage/SFImage");
    byte[] byteArray1 = {32, 0};
    ConstSFImage testConstSFImage = new ConstSFImage(1, 1, 1, byteArray1);
    System.out.println("width = " + testConstSFImage.getWidth());
    System.out.println("height = " + testConstSFImage.getHeight());
    System.out.println("components = " + testConstSFImage.getComponents());
    byte[] testPixels = new byte[2];
    testConstSFImage.getPixels(testPixels);
    System.out.println("testConstSFImage = " + testConstSFImage);
    SFImage testSFImage1 = new SFImage();
    System.out.println("width = " + testSFImage1.getWidth());
    System.out.println("height = " + testSFImage1.getHeight());
    System.out.println("components = " + testSFImage1.getComponents());
    testPixels = new byte[2];
    testSFImage1.getPixels(testPixels);
    testSFImage1.setValue(1, 1, 1, byteArray1);
    System.out.println("testSFImage (after first set) = " + testSFImage1);
    testSFImage1.setValue(testConstSFImage);
    System.out.println("testSFImage (after second set) = " + testSFImage1);
    SFImage testSFImage2 = new SFImage(1, 1, 1, byteArray1);
    testSFImage1.setValue(testSFImage2);
    System.out.println("testSFImage (after third set) = " + testSFImage1);


    /* Test ConstSFInt32/SFInt32 methods */
    System.out.println("Test ConstSFInt32/SFInt32");
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
    System.out.println("Test ConstSFNode/SFNode");
    SFNode testSFNode1 = (SFNode) getField("testTransform");
    BaseNode testBaseNode = testSFNode1.getValue();
    System.out.println("BaseNode type = " + testBaseNode.getType());
    Node testNode = (Node)testBaseNode;
    System.out.println("Node = " + testNode);
    Browser testBrowser = testBaseNode.getBrowser();
    ConstSFNode testConstSFNode1 = new ConstSFNode(testBaseNode);
    BaseNode testBaseNode2 = testConstSFNode1.getValue();
    System.out.println("testConstSFNode1 = " + testConstSFNode1);

    SFNode testSFNode2 = (SFNode) getField("testTouchSensor");

    /* Test ConstSFRotation/SFRotation methods */
    System.out.println("Test ConstSFRotation/SFRotation");
    ConstSFRotation testConstSFRotation =
      new ConstSFRotation(1.0f, 0.0f, 0.0f, 1.0f);
    float[] testRot = new float[4];
    testConstSFRotation.getValue(testRot);
    System.out.println("testConstSFRotation = " + testConstSFRotation);
    SFRotation testSFRotation1 = new SFRotation();
    SFRotation testSFRotation2 = new SFRotation(0.0f, 1.0f, 0.0f, 3.0f);
    testRot = new float[4];
    testSFRotation1.getValue(testRot);
    testSFRotation1.setValue(0.0f, 0.0f, 1.0f, 3.0f);
    System.out.println("testSFRotation1 (after first set) = " + 
                       testSFRotation1);
    testSFRotation1.setValue(testConstSFRotation);
    System.out.println("testSFRotation1 (after second set) = " +
                       testSFRotation1);
    testSFRotation1.setValue(testSFRotation2);
    System.out.println("testSFRotation1 (after third set) = " + 
                       testSFRotation1);

    /* Test ConstSFString/SFString methods */
    System.out.println("Test ConstSFString/SFString");
    SFString testString = (SFString) getField("testString");
    testString.setValue("testString2");
    System.out.println("testString = " + testString);
    SFString testSFString1 = new SFString();
    SFString testSFString2 = new SFString("testing");
    testString.setValue(testSFString1);
    System.out.println("testSFString = " + testString.getValue());
    ConstSFString testConstSFString = new ConstSFString("ConstSFString");
    System.out.println("testConstSFString = " + testConstSFString);
    System.out.println("testConstSFString = " + testConstSFString.getValue());
    testString.setValue(testConstSFString);

    /* Test ConstSFTime/SFTime methods */
    System.out.println("Test ConstSFTime/SFTime");
    ConstSFTime testConstSFTime = new ConstSFTime(500.0d);
    System.out.println("testConstSFTime = " + testConstSFTime.getValue());
    System.out.println("testConstSFTime = " + testConstSFTime);
    SFTime testSFTime1 = new SFTime();
    SFTime testSFTime2 = new SFTime(25.0d);
    System.out.println("testSFTime1 = " + testSFTime2.getValue());
    testSFTime1.setValue(300.0d);
    testSFTime2.setValue(testConstSFTime);
    testSFTime1.setValue(testSFTime2);
    System.out.println("testSFTime1 = " + testSFTime1);

    /* Test ConstSFVec2f/SFVec2f methods */
    System.out.println("Test ConstSFVec2f/SFVec2f");
    ConstSFVec2f testConstSFVec2f = new ConstSFVec2f(10, 20);
    float[] sfvec2f = new float[2];
    testConstSFVec2f.getValue(sfvec2f);
    System.out.println("testConstSFVec2f.getValue = " + sfvec2f[0] + "," +
                       sfvec2f[1]);
    System.out.println("testConstSFVec2f.getX = " + testConstSFVec2f.getX());
    System.out.println("testConstSFVec2f.getY = " + testConstSFVec2f.getY());
    System.out.println("testConstSFVec2f = " + testConstSFVec2f);
    SFVec2f testSFVec2f1 = new SFVec2f();
    SFVec2f testSFVec2f2 = new SFVec2f(10, 20);
    float vec2s[] = new float[2];
    testSFVec2f2.getValue(vec2s);
    System.out.println("testSFVec2f2.getValue = " + vec2s[0] + "," +
                       vec2s[1]);
    System.out.println("testSFVec2f2.getX = " + testSFVec2f2.getX());
    System.out.println("testSFVec2f2.getY = " + testSFVec2f2.getY());
    testSFVec2f1.setValue(testSFVec2f2);
    testSFVec2f2.setValue(testConstSFVec2f);
    testSFVec2f2.setValue(vec2s);
    testSFVec2f2.setValue(40, 30);
    System.out.println("testSFVec2f1 = " + testSFVec2f1);

    /* Test ConstSFVec3f/SFVec3f methods */
    System.out.println("Test ConstSFVec3f/SFVec3f");
    ConstSFVec3f testConstSFVec3f = new ConstSFVec3f(10, 20, 30);
    float[] sfvec3f = new float[3];
    testConstSFVec3f.getValue(sfvec3f);
    System.out.println("testConstSFVec3f.getValue = " + sfvec3f[0] + "," +
                       sfvec3f[1] + "," + sfvec3f[2]);
    System.out.println("testConstSFVec3f.getX = " + testConstSFVec3f.getX());
    System.out.println("testConstSFVec3f.getY = " + testConstSFVec3f.getY());
    System.out.println("testConstSFVec3f.getZ = " + testConstSFVec3f.getZ());
    System.out.println("testConstSFVec3f = " + testConstSFVec3f);
    SFVec3f testSFVec3f1 = new SFVec3f();
    SFVec3f testSFVec3f2 = new SFVec3f(10, 20, 30);
    float[] vec3s = new float[3];
    testSFVec3f2.getValue(vec3s);
    System.out.println("testSFVec3f2.getValue = " + vec3s[0] + "," +
                       vec3s[1] + "," + vec3s[2]);
    System.out.println("testSFVec3f2.getX = " + testSFVec3f2.getX());
    System.out.println("testSFVec3f2.getY = " + testSFVec3f2.getY());
    System.out.println("testSFVec3f2.getZ = " + testSFVec3f2.getZ());
    testSFVec3f1.setValue(testSFVec3f2);
    testSFVec3f2.setValue(testConstSFVec3f);
    testSFVec3f2.setValue(vec3s);
    testSFVec3f2.setValue(40, 30, 20);
    System.out.println("testSFVec3f1 = " + testSFVec3f1);

    /* Test ConstMFColor/MFColor methods */
    System.out.println("Test ConstMFColor/MFColor");
    float[] colorArray1 = {1.0f, 0.0f, 1.0f};
    float[] colorArray2 = {0.0f, 0.0f, 1.0f};
    float[] colorArray3 = {1.0f, 1.0f, 0.0f};
    float[][] colorArrays = {colorArray1, colorArray2, colorArray3};
    ConstMFColor testConstMFColor1= new ConstMFColor(colorArrays);
    if (testConstMFColor1.getSize() != 3)
      System.out.println("ConstMFColor.getSize() failed");
    ConstMFColor testConstMFColor2 = new ConstMFColor(colorArray1);
    ConstMFColor testConstMFColor3 = new ConstMFColor(1, colorArray2);
    float[][] result1 = new float[3][3];
    testConstMFColor1.getValue(result1);
    float[] result2 = new float[9];
    testConstMFColor1.getValue(result2);
    float[] result3 = new float[3];
    testConstMFColor1.get1Value(2, result3);
    SFColor result4 = new SFColor();
    testConstMFColor1.get1Value(1, result4);
    System.out.println("ConstMFColor = " + testConstMFColor1);
    MFColor testMFColor1 = new MFColor();
    MFColor testMFColor2 = new MFColor(colorArrays);
    MFColor testMFColor3 = new MFColor(colorArray2);
    MFColor testMFColor4 = new MFColor(3, colorArray3);
    if (testMFColor3.getSize() != 1)
      System.out.println("MFColor.getSize() failed");
    System.out.println("testMFColor2(pre-delete) = " + testMFColor2);
    testMFColor2.delete(2);
    System.out.println("testMFColor2(post-delete) = " + testMFColor2);
    System.out.println("testMFColor3(before clear) = " + testMFColor3);
    testMFColor3.clear();
    System.out.println("testMFColor3(after clear) = " + testMFColor3);
    System.out.println("testMFColor4 = " + testMFColor4);
    testMFColor2.getValue(result2);
    testMFColor2.getValue(result1);
    System.out.println("testMFColor2[0] = " + result2[0] + "," + result2[1] +
                       "," + result2[2]);
    testMFColor2.get1Value(0, result2);
    System.out.println("testMFColor2[0] = " + result2[0] + "," + result2[1] +
                       "," + result2[2]);
    System.out.println("testMFColor2[0] = " + result1[0][0] + "," + 
                       result1[0][1] + "," + result1[0][2]);
    SFColor result5 = new SFColor();
    testMFColor2.get1Value(0, result5);
    System.out.println("testMFColor2[0] = " + result5); 
    testMFColor3.setValue(colorArrays);
    System.out.println("testMFColor3 = " + testMFColor3);
    testMFColor1.setValue(colorArray2);
    System.out.println("testMFColor1 = " + testMFColor1);
    testMFColor1.setValue(0, colorArray1);
    System.out.println("testMFColor1 = " + testMFColor1);
    testMFColor2.setValue(testMFColor1);
    System.out.println("testMFColor2 = " + testMFColor2);
    testMFColor1.setValue(testConstMFColor2);
    System.out.println("testMFColor1 = " + testMFColor1);
    testMFColor3.set1Value(1, testConstSFColor);
    System.out.println("testMFColor3 = " + testMFColor3);
    testMFColor3.set1Value(0, testSFColor);
    System.out.println("testMFColor3 = " + testMFColor3);
    testMFColor3.set1Value(2, 1.0f, 1.0f, 1.0f);
    System.out.println("testMFColor3 = " + testMFColor3);
    testMFColor3.addValue(testConstSFColor);
    System.out.println("testMFColor3 = " + testMFColor3);
    testMFColor3.addValue(testSFColor);
    System.out.println("testMFColor3 = " + testMFColor3);
    testMFColor3.addValue(0.0f, 1.0f, 0.0f);
    System.out.println("testMFColor3 = " + testMFColor3);
    testMFColor3.insertValue(0, testConstSFColor);
    System.out.println("testMFColor3 = " + testMFColor3);
    testMFColor3.insertValue(2, testSFColor);
    System.out.println("testMFColor3 = " + testMFColor3);
    testMFColor3.insertValue(1, 0.0f, 0.0f, 0.0f);
    System.out.println("testMFColor3 = " + testMFColor3);

    /* Test ConstMFFloat/MFFloat methods */
    System.out.println("Test ConstMFFloat/MFFloat");
    float[] floatArray1 = { 3.0f, 4.0f, 5.0f };
    float[] floatArray2 = { 7.0f, 2.0f };
    ConstMFFloat testConstMFFloat_1 = new ConstMFFloat(2, floatArray2);
    ConstMFFloat testConstMFFloat_2 = new ConstMFFloat(floatArray1);
    if (testConstMFFloat_2.getSize() != 3)
      System.out.println("ConstMFFloat.getSize failed");
    float[] floatResult = new float[3];
    testConstMFFloat_2.getValue(floatResult);
    System.out.println("testConstMFFloat_2.getValue = " + floatResult[0] +
                       "," + floatResult[1] + "," + floatResult[2]);
    System.out.println("testConstMFFloat_2[1] = " +
                       testConstMFFloat_2.get1Value(1));
    System.out.println("testConstMFFloat_1 = " + testConstMFFloat_1);
    MFFloat testMFFloat_1 = new MFFloat(2, floatArray2);
    MFFloat testMFFloat_2 = new MFFloat(floatArray1);
    if (testMFFloat_2.getSize() != 3)
      System.out.println("MFFloat.getSize failed");
    testMFFloat_2.getValue(floatResult);
    System.out.println("testMFFloat_2.getValue = " + floatResult[0] +
                       "," + floatResult[1] + "," + floatResult[2]);
    System.out.println("testMFFloat_2[1] = " +
                       testMFFloat_2.get1Value(1));
    testMFFloat_1.setValue(floatArray1);
    System.out.println("testMFFloat_1 = " + testMFFloat_1);
    testMFFloat_1.setValue(2, floatArray2);
    System.out.println("testMFFloat_1 = " + testMFFloat_1);
    testMFFloat_1.setValue(testMFFloat_2);
    System.out.println("testMFFloat_1 = " + testMFFloat_1);
    testMFFloat_1.setValue(testConstMFFloat_1);
    System.out.println("testMFFloat_1 = " + testMFFloat_1);
    testMFFloat_1.set1Value(1, testConstSFFloat);
    System.out.println("testMFFloat_1 = " + testMFFloat_1);
    testMFFloat_1.set1Value(0, testSFFloat);
    System.out.println("testMFFloat_1 = " + testMFFloat_1);
    testMFFloat_1.set1Value(1, 12.0f);
    System.out.println("testMFFloat_1 = " + testMFFloat_1);
    testMFFloat_1.addValue(34.0f);
    System.out.println("testMFFloat_1 = " + testMFFloat_1);
    testMFFloat_1.addValue(testConstSFFloat);
    System.out.println("testMFFloat_1 = " + testMFFloat_1);
    testMFFloat_1.addValue(testSFFloat);
    System.out.println("testMFFloat_1 = " + testMFFloat_1);
    testMFFloat_1.insertValue(1, 99.0f);
    System.out.println("testMFFloat_1 = " + testMFFloat_1);
    testMFFloat_1.insertValue(1, testSFFloat);
    System.out.println("testMFFloat_1 = " + testMFFloat_1);
    testMFFloat_1.insertValue(1, testConstSFFloat);
    System.out.println("testMFFloat_1 = " + testMFFloat_1);

    /* Test ConstMFInt32/MFInt32 methods */
    System.out.println("Test ConstMFint32/MFInt32");
    int[] intArray1 = { 3, 4, 5 };
    int[] intArray2 = { 7, 2 };
    ConstMFInt32 testConstMFInt32_1 = new ConstMFInt32(2, intArray2);
    ConstMFInt32 testConstMFInt32_2 = new ConstMFInt32(intArray1);
    if (testConstMFInt32_2.getSize() != 3)
      System.out.println("ConstMFInt32.getSize failed");
    int[] intResult = new int[3];
    testConstMFInt32_2.getValue(intResult);
    System.out.println("testConstMFInt32_2.getValue = " + intResult[0] +
                       "," + intResult[1] + "," + intResult[2]);
    System.out.println("testConstMFInt32_2[1] = " +
                       testConstMFInt32_2.get1Value(1));
    System.out.println("testConstMFInt32_1 = " + testConstMFInt32_1);
    MFInt32 testMFInt32_1 = new MFInt32(2, intArray2);
    MFInt32 testMFInt32_2 = new MFInt32(intArray1);
    if (testMFInt32_2.getSize() != 3)
      System.out.println("MFInt32.getSize failed");
    testMFInt32_2.getValue(intResult);
    System.out.println("testMFInt32_2.getValue = " + intResult[0] +
                       "," + intResult[1] + "," + intResult[2]);
    System.out.println("testMFInt32_2[1] = " +
                       testMFInt32_2.get1Value(1));
    System.out.println("testMFInt32_1 = " + testMFInt32_1);

    /* Test ConstMFNode/MFNode methods */
    System.out.println("Test ConstMFNode/MFNode");

    /* Test ConstMFRotation/MFRotation methods */
    System.out.println("Test ConstMFRotation/MFRotation");

    /* Test ConstMFString/MFString methods */
    System.out.println("Test ConstMFString/MFString");
    String[] stringArray = {"test1", "test2", "test3", "test4"};
    ConstMFString testConstMFString1 = new ConstMFString(stringArray);
    ConstMFString testConstMFString2 = new ConstMFString(4, stringArray);
    String[] resultString1 = new String[4];
    testConstMFString1.getValue(resultString1);
    System.out.println("testConstMFString1[0] = " + resultString1[0]);
    System.out.println("testConstMFString1[1] = " + 
                       testConstMFString1.get1Value(1));
    System.out.println("Size of testConstMFString2 = " + 
                       testConstMFString2.getSize());
    System.out.println("testConstMFString2 = " + testConstMFString2);
    MFString testMFString1 = new MFString();
    MFString testMFString2 = new MFString(4, stringArray);
    MFString testMFString3 = new MFString(stringArray);
    String[] stringArray2 = new String[4];
    testMFString2.getValue(stringArray2);
    System.out.println("testMFString2[0] = " + testMFString2.get1Value(0));
    System.out.println("testMFString2 = " + testMFString2);

    /* Test ConstMFTime/MFTime methods */
    System.out.println("Test ConstMFTime/MFTime");

    /* Test ConstMFVec2f/MFVec2f methods */
    System.out.println("Test ConstMFVec2f/MFVec2f");

    /* Test ConstMFVec3f/MFVec3f methods */
    System.out.println("Test ConstMFVec3f/MFVec3f");
 
    System.out.println("exit initialize method");
   }
   catch(Exception e)
   {
     System.out.println("Exception caught in initialize method");
     e.printStackTrace();
   }
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

      if (event.getName().equals("isActive"))
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
