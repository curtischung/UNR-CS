import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import javax.swing.event.*;
import java.io.*;
import java.util.stream.Stream;
import java.awt.Color;

public class WindowApplication extends JFrame
{
    private static WindowApplication app;
    protected ColorDisplay drawTest;
    protected JButton buttonSave;
    protected JButton buttonReset;
    protected JButton redPlus;
    
    protected JButton greenPlus;
    protected JButton bluePlus;
    protected JButton redMinus;
    protected JButton greenMinus;
    protected JButton blueMinus;

    protected static JList colorsList;
    
    protected JLabel labelRed;
    protected JLabel labelBlue;
    protected JLabel labelGreen;
    protected static JTextField tfRed;
    protected static JTextField tfGreen;
    protected static JTextField tfBlue;

    private int red = 0;
    private int green = 0;
    private int blue = 0;

    protected String redString = Integer.toString(red);
    protected String greenString = Integer.toString(green);
    protected String blueString = Integer.toString(blue);

    protected static Color myColor;
    protected int index = 0;

    static int[] updateRed = new int[11];
    static int[] updateGreen = new int[11];
    static int[] updateBlue = new int[11];
    static String[] colorNames = new String[11];


class ColorDisplay extends JComponent
{
    public void paint(Graphics g) 
    {
        Dimension d = getSize();
        myColor = new Color(red, green, blue);

        g.setColor(myColor);
        g.fillRect(1, 1, d.width-50, d.height-50);

    }
}
    public static void main (String argv []) throws IOException
    {
        app = new WindowApplication("Color Sampler");
        fileRead();
    }
    public WindowApplication(String title) 
    {
        super(title);
        //list of color select
        colorsList = new JList();
        colorsList.addListSelectionListener(new ListHandler());
        setSize(350, 150);
        addWindowListener(new WindowDestroyer());

        buttonSave = new JButton("Save");
        buttonReset = new JButton("Reset");

        //GUI Interface Display

        drawTest = new ColorDisplay();

        redPlus = new JButton("+");
        greenPlus = new JButton("+");
        bluePlus = new JButton("+");
        redMinus = new JButton ("-");
        greenMinus = new JButton ("-");
        blueMinus = new JButton ("-");

        tfRed = new JTextField("");
        tfGreen = new JTextField("");
        tfBlue = new JTextField("");

        labelRed = new JLabel("Red:");
        labelBlue = new JLabel("Blue:");
        labelGreen = new JLabel("Green:");

        //Action Listeners

        redPlus.addActionListener(new ActionHandler());
        greenPlus.addActionListener(new ActionHandler());
        bluePlus.addActionListener(new ActionHandler());
        redMinus.addActionListener(new ActionHandler());
        greenMinus.addActionListener(new ActionHandler());
        blueMinus.addActionListener(new ActionHandler());
        tfRed.addActionListener(new ActionHandler());
        tfGreen.addActionListener(new ActionHandler());
        tfBlue.addActionListener(new ActionHandler());
        buttonSave.addActionListener(new ActionHandler());
        buttonReset.addActionListener(new ActionHandler());

        //set Null to Manually specifiy the arragement sof items on pane
        getContentPane().setLayout(null);

        getContentPane().add(drawTest);
        getContentPane().add(buttonSave);
        getContentPane().add(buttonReset);
        getContentPane().add(colorsList);
        getContentPane().add(labelRed);
        getContentPane().add(labelGreen);
        getContentPane().add(labelBlue);
        getContentPane().add(tfRed);
        getContentPane().add(tfGreen);
        getContentPane().add(tfBlue);
        getContentPane().add(redPlus);
        getContentPane().add(greenPlus);
        getContentPane().add(bluePlus);
        getContentPane().add(redMinus);
        getContentPane().add(greenMinus);
        getContentPane().add(blueMinus);

        drawTest.setBounds(10, 10, 270, 200);
        colorsList.setBounds(258, 10, 110, 300);
        buttonSave.setBounds(30, 320, 70, 30);
        buttonReset.setBounds(120, 320, 70, 30);

        labelRed.setBounds(10, 170, 40, 30);
        labelGreen.setBounds(10, 220, 40, 30);
        labelBlue.setBounds(10, 270, 40, 30);

        tfRed.setBounds(60, 170, 40, 30);
        tfGreen.setBounds(60, 220, 40, 30);
        tfBlue.setBounds(60, 270, 40, 30);

        redPlus.setBounds(110, 170, 50, 30);
        greenPlus.setBounds(110, 220, 50, 30);
        bluePlus.setBounds(110, 270, 50, 30);

        redMinus.setBounds(170, 170, 50, 30);
        greenMinus.setBounds(170, 220, 50, 30);
        blueMinus.setBounds(170, 270, 50, 30);

        setBounds(100, 100, 400, 400);
        setVisible(true);
        repaint();

    }
    public class WindowDestroyer implements WindowListener  
    {
        public void windowClosing(WindowEvent e) 
        {  
            System.exit(0);  
            fileWrite();
        }
        public void windowActivated(WindowEvent e) {} 
        public void windowClosed(WindowEvent e) {} 
        public void windowDeactivated(WindowEvent e) {} 
        public void windowDeiconified(WindowEvent e) {} 
        public void windowIconified(WindowEvent e) {} 
        public void windowOpened(WindowEvent e) {} 
    } 

    public class ActionHandler implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            if (e.getSource() == redPlus && red < 255) {
                red += 5;
                System.out.println( "red" + red);
                tfRed.setText(Integer.toString(red));
                app.setTitle("Color Sampler*");	
                repaint();
            } else
            if (e.getSource() == greenPlus && green < 255) {
                green += 5;
                System.out.println("green" + green);
                tfGreen.setText(Integer.toString(green));
                app.setTitle("Color Sampler*");	
                repaint();
            } else
            if (e.getSource() == bluePlus && blue < 255) {
                blue += 5;
                System.out.println("blue" + blue);
                tfBlue.setText(Integer.toString(blue));
                app.setTitle("Color Sampler*");	
                repaint();
            } else
            if (e.getSource() == redMinus && red != 0) {
                red -= 5;
                System.out.println("red" + red);
                tfRed.setText(Integer.toString(red));
                app.setTitle("Color Sampler*");	
                repaint();
            } else
            if (e.getSource() == greenMinus && green != 0) {
                green -= 5;
                System.out.println("green" + green);
                tfGreen.setText(Integer.toString(green));
                app.setTitle("Color Sampler*");	
                repaint();
            } else
            if (e.getSource() == blueMinus && blue != 0) {
                blue -= 5;
                System.out.println("blue" + blue);
                tfBlue.setText(Integer.toString(blue));
                app.setTitle("Color Sampler*");	
                repaint();
            } else
            if (e.getSource() == tfRed) {
                int n = Integer.parseInt(tfRed.getText());
                red = n;
                tfRed.setText(Integer.toString(red));
                app.setTitle("Color Sampler*");	
                repaint();
            } else
            if (e.getSource() == tfGreen) {
                int n = Integer.parseInt(tfGreen.getText());
                green = n;
                tfGreen.setText(Integer.toString(green));
                app.setTitle("Color Sampler*");	
                repaint();
            } else
            if (e.getSource() == tfBlue) {
                int n = Integer.parseInt(tfBlue.getText());
                blue = n;
                tfBlue.setText(Integer.toString(blue));
                app.setTitle("Color Sampler*");	
                repaint();
            } else
            if (e.getSource() == buttonSave) {
                updateRed[index] = red;
                updateGreen[index] = green;
                updateBlue[index] = blue;
                fileWrite();
                app.setTitle("Color Sampler");
            }
            if (e.getSource() == buttonReset) {
                myColor = new Color(updateRed[index], updateGreen[index], updateBlue[index]);
                tfRed.setText(Integer.toString(updateRed[index]));
                tfGreen.setText(Integer.toString(updateGreen[index]));
                tfBlue.setText(Integer.toString(updateBlue[index]));
                red = updateRed[index];
                green = updateGreen[index];
                blue = updateBlue[index];
                app.setTitle("Color Sampler");
                repaint();
            }
        }
    }
    public static void fileRead()
    {
        try 
        {
        int i = 0;
        FileInputStream stream = new FileInputStream("Colors.txt");
        InputStreamReader reader = new InputStreamReader(stream);
        StreamTokenizer tokens = new StreamTokenizer(reader);

        //Ends when gone through every line, updates each individual RGB value arrays
        while (tokens.nextToken() != tokens.TT_EOF) 
        {
            colorNames[i] = (String) tokens.sval;
            tokens.nextToken();
            updateRed[i] = (int) tokens.nval;
            tokens.nextToken();
            updateGreen[i] = (int) tokens.nval;
            tokens.nextToken();
            updateBlue[i] = (int) tokens.nval;
            i++;
        }
        stream.close();
        colorsList.setListData(colorNames);
        }catch (IOException e)
        {
            System.out.println("error");
        }
        
    }
    public static void fileWrite()
    {
        int i = 0;
        try {
            FileInputStream istream = new FileInputStream("Colors.txt");
            InputStreamReader reader = new InputStreamReader(istream);
            StreamTokenizer tokens = new StreamTokenizer(reader);

            FileOutputStream ostream = new FileOutputStream("Colors.txt");
            PrintWriter writer = new PrintWriter(ostream);
            for(i = 0;  i < 11 ; i++)
            {
                writer.println(colorNames[i] + " " + updateRed[i] + " " + updateGreen[i] + " " + updateBlue[i]);
            }
            writer.flush();
            ostream.close();
            istream.close();
        } 
        catch (IOException e)
        {
            System.out.println("error");
        }
    }
    public class ListHandler implements ListSelectionListener
    {
        public void valueChanged (ListSelectionEvent e)
        {
            if(e.getSource() == colorsList)
            {
                index = colorsList.getSelectedIndex();
                myColor = new Color(updateRed[index], updateGreen[index], updateBlue[index]);
                tfRed.setText(Integer.toString(updateRed[index]));
                tfGreen.setText(Integer.toString(updateGreen[index]));
                tfBlue.setText(Integer.toString(updateBlue[index]));
                red = updateRed[index];
                green = updateGreen[index];
                blue = updateBlue[index];
                repaint();
            }
        }
    }
}
