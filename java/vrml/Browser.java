//
// OpenVRML
//
// Copyright 2001  Henri Manson
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

/**
 * The browser interface provides a mechanism for scripts contained by
 * Script nodes to get and set browser state.
 */
public class Browser {
    /**
     * Stores a pointer to a VrmlScene that will be used on the C++ side
     * to map calls for any of the event of the class.
     */
    private int BrowserPtr;

    public native String toString();

    /**
     * Name of the browser currently in use.
     *
     * @return name of the browser
     */
    public native String getName();

    /**
     * Version of the browser currently in use.
     *
     * @return version of the browser
     */
    public native String getVersion();

    /**
     * Returns the average navigation speed for the currently bound
     * NavigationInfo node in meters/second.
     *
     * @return current speed in metres/second
     */
    public native float getCurrentSpeed();

    /**
     * Return the current frame rate in frames/second.
     *
     * @return current frame rate
     */
    public native float getCurrentFrameRate();

    /**
     * Return the URL for the root of the currently loaded world.
     *
     * @return root URL of the currently loaded world
     */
    public native String getWorldURL();

    /**
     * Replaces the current world with the world represented by the
     * passed nodes.
     *
     * @param nodes New world
     */
    public native void replaceWorld(BaseNode[] nodes);

    /**
     * This method parses a string consisting of VRML statements, establishes
     * any PROTO and EXTERNPROTO declarations and routes, and returns an MFNode
     * value containing the set of nodes in those statements.
     *
     * @param vrmlSyntax VRML statements
     * @return Array of nodes representing the given VRML statements.
     * @throws InvalidVRMLSyntaxException If invalid syntax in string
     */
    public native BaseNode[] createVrmlFromString(String vrmlSyntax)
        throws InvalidVRMLSyntaxException;

    /**
     * Instructs the browser to load a VRML scene description from the
     * given URL or URLs. After the scene is loaded, the given event
     * is sent to the passed node.
     *
     * @param url array of URLs
     * @param node node to send event to
     * @param event event to send to node
     * @throws InvalidVRMLSyntaxException If URL contain invalid syntax
     */
    public native void createVrmlFromURL(String[] url, BaseNode node,
                                         String event)
        throws InvalidVRMLSyntaxException;

    /**
     * Add a route between the given event names for the given nodes.
     *
     * @param fromNode Node where route originates
     * @param fromEvenOut Event Out where route originates
     * @param toNode Node where route ends
     * @param toEventIn Event In where route ends
     */
    public native void addRoute(BaseNode fromNode, String fromEventOut,
                                BaseNode toNode, String toEventIn);

    /**
     * Delete a route between the given event names for the given nodes.
     *
     * @param fromNode Node where route originates
     * @param fromEvenOut Event Out where route originates
     * @param toNode Node where route ends
     * @param toEventIn Event In where route ends
     */
    public native void deleteRoute(BaseNode fromNode, String fromEventOut,
                                   BaseNode toNode, String toEventIn);

    /**
     * Loads the first recognized URL from the specified url field with
     * the passed parameters.
     *
     * @param url array of URLs
     * @param parameter parameters to load URL with
     * @throws InvalidVRMLSyntaxException If URL has invalid VRML syntax
     */
    public native void loadURL(String[] url, String[] parameter)
        throws InvalidVRMLSyntaxException;

    /**
     * Sets the passed string as the current description
     *
     * @param description New description
     */
    public native void setDescription(String description);
}
