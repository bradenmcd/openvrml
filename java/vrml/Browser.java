//
// OpenVRML
//
// Copyright (C) 2001  Henri Manson 
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// 

package vrml;

public class Browser 
{
  private int BrowserPtr;

  public native String toString();
 
  // Browser interface
  public native String getName();    
  public native String getVersion();    
    
  public native float getCurrentSpeed();
    
  public native float getCurrentFrameRate();
    
  public native String getWorldURL();
  public native void replaceWorld(BaseNode[] nodes);
    
  public native BaseNode[] createVrmlFromString(String vrmlSyntax);
  //     throws InvalidVRMLSyntaxException;
    
  public native void createVrmlFromURL(String[] url, BaseNode node, String event);
  //     throws InvalidVRMLSyntaxException;
    
  public native void addRoute(BaseNode fromNode, String fromEventOut,
			      BaseNode toNode, String toEventIn);
    
  public native void deleteRoute(BaseNode fromNode, String fromEventOut,
				 BaseNode toNode, String toEventIn);
    
  public native void loadURL(String[] url, String[] parameter);
  //     throws InvalidVRMLSyntaxException;
    
  public native void setDescription(String description);
}
