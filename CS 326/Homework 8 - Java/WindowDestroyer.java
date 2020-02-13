import java.awt.event.*;

public class WindowDestroyer implements WindowListener
{
    public void windowClosing(WindowEvent e) 
    {  System.exit(0);  }
    public void windowActivated(WindowEvent e) {} 
    public void windowClosed(WindowEvent e) {} 
    public void windowDeactivated(WindowEvent e) {} 
    public void windowDeiconified(WindowEvent e) {} 
    public void windowIconified(WindowEvent e) {} 
    public void windowOpened(WindowEvent e) {} 
} 