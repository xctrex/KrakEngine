import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.SwingUtilities;
import javax.swing.filechooser.FileFilter;
import javax.swing.filechooser.FileNameExtensionFilter;




public class GUI extends JFrame{
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	static String id;
	
	 public String readFile(File file) {

	        StringBuffer fileBuffer = null;
	        String fileString = null;
	        String line = null;

	        try {
	            FileReader in = new FileReader(file);
	            BufferedReader brd = new BufferedReader(in);
	            fileBuffer = new StringBuffer();

	            while ((line = brd.readLine()) != null) {
	                fileBuffer.append(line).append(
	                        System.getProperty("line.separator"));
	            }

	            in.close();
	            fileString = fileBuffer.toString();
	        } catch (IOException e) {
	            return null;
	        }
	        return fileString;
	    }

	public GUI(){
		
		setTitle("Level Creator");
		setSize(1000, 700);
		setLocationRelativeTo(null);
		setDefaultCloseOperation(EXIT_ON_CLOSE);
		
		final JFileChooser fileopen = new JFileChooser();
		FileFilter filter = new FileNameExtensionFilter("XML files", "xml");
		fileopen.addChoosableFileFilter(filter);
		
		//Create the Menu Bar
		JMenuBar menubar = new JMenuBar();
		JMenu file = new JMenu("File");
		
		JMenuItem newI = new JMenuItem("New");
		JMenuItem open = new JMenuItem("Open");
		JMenuItem save = new JMenuItem("Save");
		JMenuItem exit = new JMenuItem("Exit");
		
		//Create window Panel
		JPanel windowPanel = new JPanel();
		windowPanel.setBorder(BorderFactory.createEmptyBorder(5,5,5,5));
		windowPanel.setLayout(new GridLayout(1,2,5,5));
		
		//Create JTextPane
		final JTextPane previewPane = new JTextPane();
		previewPane.setEditable(false);
		previewPane.setText("<level id=\"" + id + "\">");
		JScrollPane scroll = new JScrollPane(previewPane);
		
		newI.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent e) {
				int n = JOptionPane.showConfirmDialog(rootPane, "Do you want to start a new file? All unsaved changes will be lost!", "New File", JOptionPane.YES_NO_OPTION);
				if(n == 0){
					id = JOptionPane.showInputDialog(null, "Enter Level ID:", "", 1);
					previewPane.setText("<level id=\"" + id + "\">");
				}
				
			}
			
		});
		
		open.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent e) {
				int ret = fileopen.showDialog(rootPane, "Open File");
				
				if(ret == JFileChooser.APPROVE_OPTION){
					File file = fileopen.getSelectedFile();
					String text = readFile(file);
					previewPane.setText(text);
				}
				
			}
			
		});
		
		save.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent e) {
				int ret = fileopen.showDialog(rootPane, "Save File");
				
				if(ret == JFileChooser.APPROVE_OPTION){
					File file = fileopen.getSelectedFile();
					PrintWriter writer;
					try {
						if(!file.getAbsolutePath().endsWith(".xml")){
							writer = new PrintWriter(file.getAbsolutePath() + ".xml", "UTF-8");
							writer.println(previewPane.getText());
							if(!previewPane.getText().endsWith("</level>")){
								writer.println("\n</level>"); 
							}
							writer.close();
						}else{
							writer = new PrintWriter(file.getAbsolutePath(), "UTF-8");
							writer.println(previewPane.getText());
							if(!previewPane.getText().endsWith("</level>")){
								writer.println("\n</level>"); 
							}
							writer.close();
						}
					} catch (FileNotFoundException e1) {
						e1.printStackTrace();
					} catch (UnsupportedEncodingException e1) {
						e1.printStackTrace();
					}	
				}
			}
			
		});
		
		exit.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent e) {
				System.exit(0);
			}
			
		});
		
		file.add(newI);
		file.add(open);
		file.add(save);
		file.add(exit);
		
		menubar.add(file);
		
		setJMenuBar(menubar);
		
		//Create main Grid Panel
		JPanel mainPanel = new JPanel();
		mainPanel.setBorder(BorderFactory.createEmptyBorder(5,5,5,5));
		mainPanel.setLayout(new GridLayout(10,1,10,5));
		
		//Object Information
		JPanel objectPanel = new JPanel();
		objectPanel.setLayout(new GridLayout(3,2,5,5));
		
		JLabel objectTitle = new JLabel("Object:");
		
		JPanel objectNamePanel = new JPanel();
		objectNamePanel.setLayout(new GridLayout(1,2,5,5));
		JLabel objectNameLabel = new JLabel("Object Name:");
		final JTextField objectName = new JTextField();
		objectNamePanel.add(objectNameLabel);
		objectNamePanel.add(objectName);
		
		objectPanel.add(objectTitle);
		objectPanel.add(new JPanel());
		objectPanel.add(objectNamePanel);
		objectPanel.add(new JPanel());
		
		mainPanel.add(objectPanel);
		
		//Create Life Component
		JPanel lifePanel = new JPanel();
		lifePanel.setLayout(new GridLayout(3,2,5,5));
		
		JLabel lifeTitle = new JLabel("Life Component:");
		final JCheckBox lifeCheck = new JCheckBox("Enabled", true);

		
		JPanel maxHPPanel = new JPanel();
		final JLabel maxHPLabel = new JLabel("     Max HP");
		final JTextField maxHP = new JTextField();
		maxHPPanel.setLayout(new GridLayout(1,2,5,5));
		maxHPPanel.add(maxHPLabel);
		maxHPPanel.add(maxHP);
		
		JPanel currentHPPanel = new JPanel();
		final JLabel currentHPLabel = new JLabel("     Current HP");
		final JTextField currentHP = new JTextField();
		currentHPPanel.setLayout(new GridLayout(1,2,5,5));
		currentHPPanel.add(currentHPLabel);
		currentHPPanel.add(currentHP);
		
		lifeCheck.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent e) {
				maxHP.setEnabled(lifeCheck.isSelected());
				maxHPLabel.setEnabled(lifeCheck.isSelected());
				currentHP.setEnabled(lifeCheck.isSelected());
				currentHPLabel.setEnabled(lifeCheck.isSelected());
				if(lifeCheck.isSelected()){
					lifeCheck.setText("Enabled");
				}else{
					lifeCheck.setText("Disabled");
				}
			}
			
			
		});
		
		lifePanel.add(lifeTitle);
		lifePanel.add(lifeCheck);
		lifePanel.add(maxHPPanel);
		lifePanel.add(currentHPPanel);
		mainPanel.add(lifePanel);
		
		//Create Transform Component
		JPanel transformPanel = new JPanel();
		transformPanel.setLayout(new GridLayout(3,1,5,5));
		
		JPanel transformTitlePanel = new JPanel();
		transformTitlePanel.setLayout(new GridLayout(1,2,5,5));
		JLabel transformTitle = new JLabel("Transform Component:");
		final JCheckBox transformCheck = new JCheckBox("Enabled", true);
		
		transformTitlePanel.add(transformTitle);
		transformTitlePanel.add(transformCheck);
		
		JPanel transformAttributePanel1 = new JPanel();
		transformAttributePanel1.setLayout(new GridLayout(1,2,5,5));
		
		JPanel xPanel = new JPanel();
		xPanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel xLabel = new JLabel("     X");
		final JTextField x = new JTextField();
		
		xPanel.add(xLabel);
		xPanel.add(x);
		
		JPanel yPanel = new JPanel();
		yPanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel yLabel = new JLabel("     Y");
		final JTextField y = new JTextField();
		
		yPanel.add(yLabel);
		yPanel.add(y);
		
		JPanel transformAttributePanel2 = new JPanel();
		transformAttributePanel2.setLayout(new GridLayout(1,2,5,5));
		
		JPanel zPanel = new JPanel();
		zPanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel zLabel = new JLabel("     Z");
		final JTextField z = new JTextField();
		
		zPanel.add(zLabel);
		zPanel.add(z);
		
		JPanel rotationPanel = new JPanel();
		rotationPanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel rotationLabel = new JLabel("     Rotation");
		final JTextField rotation = new JTextField();
		
		rotationPanel.add(rotationLabel);
		rotationPanel.add(rotation);
		
		transformCheck.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent e) {
				x.setEnabled(transformCheck.isSelected());
				xLabel.setEnabled(transformCheck.isSelected());
				y.setEnabled(transformCheck.isSelected());
				yLabel.setEnabled(transformCheck.isSelected());
				z.setEnabled(transformCheck.isSelected());
				zLabel.setEnabled(transformCheck.isSelected());
				rotation.setEnabled(transformCheck.isSelected());
				rotationLabel.setEnabled(transformCheck.isSelected());
				
				if(transformCheck.isSelected()){
					transformCheck.setText("Enabled");
				}else{
					transformCheck.setText("Disabled");
				}
			}
			
		});
		
		transformAttributePanel1.add(xPanel);
		transformAttributePanel1.add(yPanel);
		transformAttributePanel2.add(zPanel);
		transformAttributePanel2.add(rotationPanel);
		
		transformPanel.add(transformTitlePanel);
		transformPanel.add(transformAttributePanel1);
		transformPanel.add(transformAttributePanel2);
		mainPanel.add(transformPanel);
		
		//Create Physics Component
		JPanel physicsPanel1 = new JPanel();
		physicsPanel1.setLayout(new GridLayout(3,1,5,5));
		
		JPanel physicsPanel2 = new JPanel();
		physicsPanel2.setLayout(new GridLayout(3,1,5,5));
		
		JPanel physicsTitlePanel = new JPanel();
		physicsTitlePanel.setLayout(new GridLayout(1,2,5,5));
		JLabel physicsTitle = new JLabel("Physics Component:");
		final JCheckBox physicsCheck = new JCheckBox("Enabled", true);
		
		physicsTitlePanel.add(physicsTitle);
		physicsTitlePanel.add(physicsCheck);
		
		JPanel physicsAttributePanel1 = new JPanel();
		physicsAttributePanel1.setLayout(new GridLayout(1,2,5,5));
		
		JPanel velXPanel = new JPanel();
		velXPanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel velXLabel = new JLabel("     Velocity X");
		final JTextField velX = new JTextField();
		
		velXPanel.add(velXLabel);
		velXPanel.add(velX);
		
		JPanel velYPanel = new JPanel();
		velYPanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel velYLabel = new JLabel("     Velocity Y");
		final JTextField velY = new JTextField();
		
		velYPanel.add(velYLabel);
		velYPanel.add(velY);
		
		JPanel physicsAttributePanel2 = new JPanel();
		physicsAttributePanel2.setLayout(new GridLayout(1,2,5,5));
		
		JPanel speedPanel = new JPanel();
		speedPanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel speedLabel = new JLabel("     Speed");
		final JTextField speed = new JTextField();
		
		speedPanel.add(speedLabel);
		speedPanel.add(speed);
		
		JPanel massPanel = new JPanel();
		massPanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel massLabel = new JLabel("     Mass");
		final JTextField mass = new JTextField();
		
		massPanel.add(massLabel);
		massPanel.add(mass);
		
		JPanel physicsAttributePanel3 = new JPanel();
		physicsAttributePanel3.setLayout(new GridLayout(1,2,5,5));
		
		JPanel frictionPanel = new JPanel();
		frictionPanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel frictionLabel = new JLabel("     Friction");
		final JTextField friction = new JTextField();
		
		frictionPanel.add(frictionLabel);
		frictionPanel.add(friction);
		
		JPanel BodyTypePanel = new JPanel();
		BodyTypePanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel BodyTypeLabel = new JLabel("     Body Type");
		final JTextField BodyType = new JTextField();
		
		BodyTypePanel.add(BodyTypeLabel);
		BodyTypePanel.add(BodyType);
		
		JPanel physicsAttributePanel4 = new JPanel();
		physicsAttributePanel4.setLayout(new GridLayout(1,2,5,5));
		
		JPanel PhySizeXPanel = new JPanel();
		PhySizeXPanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel PhySizeXLabel = new JLabel("     Size X");
		final JTextField PhySizeX = new JTextField();
		
		PhySizeXPanel.add(PhySizeXLabel);
		PhySizeXPanel.add(PhySizeX);
		
		JPanel PhySizeYPanel = new JPanel();
		PhySizeYPanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel PhySizeYLabel = new JLabel("     Size Y");
		final JTextField PhySizeY = new JTextField();
		
		PhySizeYPanel.add(PhySizeYLabel);
		PhySizeYPanel.add(PhySizeY);
		
		JPanel physicsAttributePanel5 = new JPanel();
		physicsAttributePanel5.setLayout(new GridLayout(1,2,5,5));
		
		JPanel PhySizeZPanel = new JPanel();
		PhySizeZPanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel PhySizeZLabel = new JLabel("     Size Z");
		final JTextField PhySizeZ = new JTextField();
		
		PhySizeZPanel.add(PhySizeZLabel);
		PhySizeZPanel.add(PhySizeZ);
		
		physicsCheck.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent e) {
				velX.setEnabled(physicsCheck.isSelected());
				velXLabel.setEnabled(physicsCheck.isSelected());
				velY.setEnabled(physicsCheck.isSelected());
				velYLabel.setEnabled(physicsCheck.isSelected());
				speed.setEnabled(physicsCheck.isSelected());
				speedLabel.setEnabled(physicsCheck.isSelected());
				mass.setEnabled(physicsCheck.isSelected());
				massLabel.setEnabled(physicsCheck.isSelected());
				friction.setEnabled(physicsCheck.isSelected());
				frictionLabel.setEnabled(physicsCheck.isSelected());
				BodyType.setEnabled(physicsCheck.isSelected());
				BodyTypeLabel.setEnabled(physicsCheck.isSelected());
				PhySizeX.setEnabled(physicsCheck.isSelected());
				PhySizeXLabel.setEnabled(physicsCheck.isSelected());
				PhySizeY.setEnabled(physicsCheck.isSelected());
				PhySizeYLabel.setEnabled(physicsCheck.isSelected());
				PhySizeZ.setEnabled(physicsCheck.isSelected());
				PhySizeZLabel.setEnabled(physicsCheck.isSelected());
				
				if(physicsCheck.isSelected()){
					physicsCheck.setText("Enabled");
				}else{
					physicsCheck.setText("Disabled");
				}
				
			}
			
		});
		
		physicsAttributePanel1.add(velXPanel);
		physicsAttributePanel1.add(velYPanel);
		physicsAttributePanel2.add(speedPanel);
		physicsAttributePanel2.add(massPanel);
		physicsAttributePanel3.add(frictionPanel);
		physicsAttributePanel3.add(BodyTypePanel);
		physicsAttributePanel4.add(PhySizeXPanel);
		physicsAttributePanel4.add(PhySizeYPanel);
		physicsAttributePanel5.add(PhySizeZPanel);
		physicsAttributePanel5.add(new JPanel());
		
		physicsPanel1.add(physicsTitlePanel);
		physicsPanel1.add(physicsAttributePanel1);
		physicsPanel1.add(physicsAttributePanel2);
		physicsPanel2.add(physicsAttributePanel3);
		physicsPanel2.add(physicsAttributePanel4);
		physicsPanel2.add(physicsAttributePanel5);
		mainPanel.add(physicsPanel1);
		mainPanel.add(physicsPanel2);
		
		//Create Model Component
		JPanel modelPanel = new JPanel();
		modelPanel.setLayout(new GridLayout(3,1,5,5));
		
		JPanel modelTitlePanel = new JPanel();
		modelTitlePanel.setLayout(new GridLayout(1,2,5,5));
		JLabel modelLabel = new JLabel("Model Component:");
		final JCheckBox modelCheck = new JCheckBox("Enabled", true);
		modelTitlePanel.add(modelLabel);
		modelTitlePanel.add(modelCheck);
		
		JPanel modelAttributePanel1 = new JPanel();
		modelAttributePanel1.setLayout(new GridLayout(1,2,5,5));
		
		JPanel sizeXPanel = new JPanel();
		sizeXPanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel sizeXLabel = new JLabel("     Size X");
		final JTextField sizeX = new JTextField();
		sizeXPanel.add(sizeXLabel);
		sizeXPanel.add(sizeX);
		
		JPanel sizeYPanel = new JPanel();
		sizeYPanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel sizeYLabel = new JLabel("     Size Y");
		final JTextField sizeY = new JTextField();
		sizeYPanel.add(sizeYLabel);
		sizeYPanel.add(sizeY);
		
		JPanel modelAttributePanel2 = new JPanel();
		modelAttributePanel2.setLayout(new GridLayout(1,2,5,5));
		
		JPanel sizeZPanel = new JPanel();
		sizeZPanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel sizeZLabel = new JLabel("     Size Z");
		final JTextField sizeZ = new JTextField();
		sizeZPanel.add(sizeZLabel);
		sizeZPanel.add(sizeZ);
		
		modelCheck.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent e) {
				sizeX.setEnabled(modelCheck.isSelected());
				sizeXLabel.setEnabled(modelCheck.isSelected());
				sizeY.setEnabled(modelCheck.isSelected());
				sizeYLabel.setEnabled(modelCheck.isSelected());
				sizeZ.setEnabled(modelCheck.isSelected());
				sizeZLabel.setEnabled(modelCheck.isSelected());
				if(modelCheck.isSelected()){
					modelCheck.setText("Enabled");
				}else{
					modelCheck.setText("Disabled");
				}
			}
			
		});
		
		modelAttributePanel1.add(sizeXPanel);
		modelAttributePanel1.add(sizeYPanel);
		
		modelAttributePanel2.add(sizeZPanel);
		modelAttributePanel2.add(new JPanel());
		
		modelPanel.add(modelTitlePanel);
		modelPanel.add(modelAttributePanel1);
		modelPanel.add(modelAttributePanel2);
		mainPanel.add(modelPanel);
		
		//Create Camera Component
		JPanel cameraPanel = new JPanel();
		cameraPanel.setLayout(new GridLayout(3,1,5,5));
		
		JPanel cameraTitlePanel = new JPanel();
		cameraTitlePanel.setLayout(new GridLayout(1,2,5,5));
		JLabel cameraTitle = new JLabel("Camera Component:");
		final JCheckBox cameraCheck = new JCheckBox("Enabled", true);
		
		cameraTitlePanel.add(cameraTitle);
		cameraTitlePanel.add(cameraCheck);
		
		JPanel cameraAttributePanel1 = new JPanel();
		cameraAttributePanel1.setLayout(new GridLayout(1,2,5,5));
		
		JPanel cameraDepthPanel = new JPanel();
		cameraDepthPanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel cameraDepthLabel = new JLabel("     Camera Depth");
		final JTextField cameraDepth = new JTextField();
		cameraDepthPanel.add(cameraDepthLabel);
		cameraDepthPanel.add(cameraDepth);
		
		JPanel cameraHeightPanel = new JPanel();
		cameraHeightPanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel cameraHeightLabel = new JLabel("     Camera Height");
		final JTextField cameraHeight = new JTextField();
		cameraHeightPanel.add(cameraHeightLabel);
		cameraHeightPanel.add(cameraHeight);
		
		JPanel cameraAttributePanel2 = new JPanel();
		cameraAttributePanel2.setLayout(new GridLayout(1,2,5,5));
		
		JPanel lookHeightPanel = new JPanel();
		lookHeightPanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel lookHeightLabel = new JLabel("     Look Height");
		final JTextField lookHeight = new JTextField();
		lookHeightPanel.add(lookHeightLabel);
		lookHeightPanel.add(lookHeight);
		
		cameraAttributePanel1.add(cameraDepthPanel);
		cameraAttributePanel1.add(cameraHeightPanel);
		
		cameraAttributePanel2.add(lookHeightPanel);
		cameraAttributePanel2.add(new JPanel());
		
		cameraCheck.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {
				cameraDepth.setEnabled(cameraCheck.isSelected());
				cameraDepthLabel.setEnabled(cameraCheck.isSelected());
				cameraHeight.setEnabled(cameraCheck.isSelected());
				cameraHeightLabel.setEnabled(cameraCheck.isSelected());
				lookHeight.setEnabled(cameraCheck.isSelected());
				lookHeightLabel.setEnabled(cameraCheck.isSelected());
				if(cameraCheck.isSelected()){
					cameraCheck.setText("Enabled");
				}else{
					cameraCheck.setText("Disabled");
				}
			}
			
		});
		
		cameraPanel.add(cameraTitlePanel);
		cameraPanel.add(cameraAttributePanel1);
		cameraPanel.add(cameraAttributePanel2);
		mainPanel.add(cameraPanel);
		
		//Create Text Component
		JPanel textPanel = new JPanel();
		textPanel.setLayout(new GridLayout(3,1,5,5));
				
		JPanel textTitlePanel = new JPanel();
		textTitlePanel.setLayout(new GridLayout(1,2,5,5));
		JLabel textLabel = new JLabel("Text Component:");
		final JCheckBox textCheck = new JCheckBox("Enabled", true);
		textTitlePanel.add(textLabel);
		textTitlePanel.add(textCheck);
				
		JPanel textAttributePanel1 = new JPanel();
		textAttributePanel1.setLayout(new GridLayout(1,2,5,5));
				
		JPanel posXPanel = new JPanel();
		posXPanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel posXLabel = new JLabel("     Position X");
		final JTextField posX = new JTextField();
		posXPanel.add(posXLabel);
		posXPanel.add(posX);
				
		JPanel posYPanel = new JPanel();
		posYPanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel posYLabel = new JLabel("     Position Y");
		final JTextField posY = new JTextField();
		posYPanel.add(posYLabel);
		posYPanel.add(posY);
				
		JPanel textAttributePanel2 = new JPanel();
		textAttributePanel2.setLayout(new GridLayout(1,2,5,5));
		
		JPanel contentPanel = new JPanel();
		contentPanel.setLayout(new GridLayout(1,2,5,5));
		final JLabel contentLabel = new JLabel("     Content");
		final JTextField content = new JTextField();
		contentPanel.add(contentLabel);
		contentPanel.add(content);
		
		textCheck.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent e) {
				posX.setEnabled(textCheck.isSelected());
				posXLabel.setEnabled(textCheck.isSelected());
				posY.setEnabled(textCheck.isSelected());
				posYLabel.setEnabled(textCheck.isSelected());
				content.setEnabled(textCheck.isSelected());
				contentLabel.setEnabled(textCheck.isSelected());
				if(textCheck.isSelected()){
					textCheck.setText("Enabled");
				}else{
					textCheck.setText("Disabled");
				}
			}
			
		});
				
		textAttributePanel1.add(posXPanel);
		textAttributePanel1.add(posYPanel);
			
		textAttributePanel2.add(contentPanel);
		textAttributePanel2.add(new JPanel());
				
		textPanel.add(textTitlePanel);
		textPanel.add(textAttributePanel1);
		textPanel.add(textAttributePanel2);
		mainPanel.add(textPanel);		
		
		//Create Audio Component
		JPanel audioPanel = new JPanel();
		audioPanel.setLayout(new GridLayout(3,2,5,5));
		
		JLabel audioTitle = new JLabel("Audio Component:");
		final JCheckBox audioCheck = new JCheckBox("Enabled", true);
		
		JPanel fileNamePanel = new JPanel();
		final JLabel fileNameLabel = new JLabel("      File");
		final JTextField fileName = new JTextField();
		fileNamePanel.setLayout(new GridLayout(1,2,5,5));
		fileNamePanel.add(fileNameLabel);
		fileNamePanel.add(fileName);
		
		audioCheck.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent e) {
				fileName.setEnabled(audioCheck.isSelected());
				fileNameLabel.setEnabled(audioCheck.isSelected());
				if(audioCheck.isSelected()){
					audioCheck.setText("Enabled");
				}else{
					audioCheck.setText("Disabled");
				}
			}
			
		});
		
		audioPanel.add(audioTitle);
		audioPanel.add(audioCheck);
		audioPanel.add(fileNamePanel);
		audioPanel.add(new JPanel());
		mainPanel.add(audioPanel);
		
		//Button Panel
		JPanel buttonPanel = new JPanel();
		buttonPanel.setLayout(new GridLayout(2,2,5,5));
		
		JButton addBtn = new JButton("Add");
		JButton clearBtn = new JButton("Clear");
		JButton saveBtn = new JButton("Save");
		JButton loadBtn = new JButton("Load");
		
		addBtn.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent e) {
				String add = previewPane.getText();
				if(!objectName.getText().equals("")){
					add += "\n<" + objectName.getText() + " type=\"gameobject\">";
					
					if(lifeCheck.isSelected()){
						add += "\n\t<Life type=\"component\"";
						
						if(!maxHP.getText().equals("")){
							add += "\n\t\tmaxHP=\"" + maxHP.getText() + "\"";
						}
						
						if(!currentHP.getText().equals("")){
							add += "\n\t\tcurrentHP=\"" + currentHP.getText() + "\"";
						}
						
						add += "\n\t/>";
					}
					
					if(transformCheck.isSelected()){
						add += "\n\t<Transform type=\"component\"";
						
						if(!x.getText().equals("")){
							add += "\n\t\tx=\"" + x.getText() + "\"";
						}
						
						if(!y.getText().equals("")){
							add += "\n\t\ty=\"" + y.getText() + "\"";
						}
						
						if(!z.getText().equals("")){
							add += "\n\t\tz=\"" + z.getText() + "\"";
						}
						
						if(!rotation.getText().equals("")){
							add += "\n\t\trotation=\"" + rotation.getText() + "\"";
						}
						
						add += "\n\t/>";
					}
					
					if(physicsCheck.isSelected()){
						add += "\n\t<Physics type=\"component\"";
						
						if(!velX.getText().equals("")){
							add += "\n\t\tvelocityX=\"" + velX.getText() + "\"";
						}
						
						if(!velY.getText().equals("")){
							add += "\n\t\tvelocityY=\"" + velY.getText() + "\"";
						}
						
						if(!speed.getText().equals("")){
							add += "\n\t\tspeed=\"" + speed.getText() + "\"";
						}
						
						if(!mass.getText().equals("")){
							add += "\n\t\tmass=\"" + mass.getText() + "\"";
						}
						
						if(!friction.getText().equals("")){
							add += "\n\t\tfriction=\"" + friction.getText() + "\"";
						}
						
						if(!BodyType.getText().equals("")){
							add += "\n\t\tBodyType=\"" + BodyType.getText() + "\"";
						}
						
						if(!PhySizeX.getText().equals("")){
							add += "\n\t\tSizeX=\"" + PhySizeX.getText() + "\"";
						}
						
						if(!PhySizeY.getText().equals("")){
							add += "\n\t\tSizeY=\"" + PhySizeY.getText() + "\"";
						}
						
						if(!PhySizeZ.getText().equals("")){
							add += "\n\t\tSizeZ=\"" + PhySizeZ.getText() + "\"";
						}
						
						add += "\n\t/>";
					}
					
					if(modelCheck.isSelected()){
						add += "\n\t<Model type=\"component\"";
						
						if(!sizeX.getText().equals("")){
							add += "\n\t\tSizeX=\"" + sizeX.getText() + "\"";
						}
						
						if(!sizeY.getText().equals("")){
							add += "\n\t\tSizeY=\"" + sizeY.getText() + "\"";
						}
						
						if(!sizeZ.getText().equals("")){
							add += "\n\t\tSizeZ=\"" + sizeZ.getText() + "\"";
						}
						
						add += "\n\t/>";
					}
					
					if(cameraCheck.isSelected()){
						add += "\n\t<Camera type=\"component\"";
						
						if(!cameraDepth.getText().equals("")){
							add += "\n\t\tCameraDepth=\"" + cameraDepth.getText() + "\"";
						}
						
						if(!cameraHeight.getText().equals("")){
							add += "\n\t\tCameraHeight=\"" + cameraHeight.getText() + "\"";
						}
						
						if(!lookHeight.getText().equals("")){
							add += "\n\t\tLookHeight=\"" + lookHeight.getText() + "\"";
						}
						
						add += "\n\t/>";
					}
					
					if(textCheck.isSelected()){
						add += "\n\t<Text type=\"component\"";
						
						if(!posX.getText().equals("")){
							add += "\n\t\tPosX=\"" + posX.getText() + "\"";
						}
						
						if(!posY.getText().equals("")){
							add += "\n\t\tPosY=\"" + posY.getText() + "\"";
						}
						
						if(!content.getText().equals("")){
							add += "\n\t\tContent=\"" + content.getText() + "\"";
						}
						
						add += "\n\t/>";
					}
					
					if(audioCheck.isSelected()){
						add += "\n\t<Audio type=\"component\"";
						
						if(!fileName.getText().equals("")){
							add += "\n\t\tfile=\"" + fileName.getText() + "\"";
						}
						
						add += "\n\t/>";
					}
					
					add += "\n</" + objectName.getText() + ">";
				}
				previewPane.setText(add);
			}
			
		});
		
		clearBtn.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent arg0) {
				objectName.setText("");
				maxHP.setText("");
				currentHP.setText("");
				x.setText("");
				y.setText("");
				z.setText("");
				rotation.setText("");
				velX.setText("");
				velY.setText("");
				speed.setText("");
				mass.setText("");
				friction.setText("");
				BodyType.setText("");
				PhySizeX.setText("");
				PhySizeY.setText("");
				PhySizeZ.setText("");
				sizeX.setText("");
				sizeY.setText("");
				sizeZ.setText("");
				cameraDepth.setText("");
				cameraHeight.setText("");
				lookHeight.setText("");
				posX.setText("");
				posY.setText("");
				content.setText("");
				fileName.setText("");
				
			}
			
		});
		
		saveBtn.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent arg0) {
				int ret = fileopen.showDialog(rootPane, "Save File");
				
				if(ret == JFileChooser.APPROVE_OPTION){
					File file = fileopen.getSelectedFile();
					PrintWriter writer;
					try {
						if(!file.getAbsolutePath().endsWith(".xml")){
							writer = new PrintWriter(file.getAbsolutePath() + ".xml", "UTF-8");
							writer.println(previewPane.getText());
							if(!previewPane.getText().endsWith("</level>")){
								writer.println("\n</level>"); 
							}
							writer.close();
						}else{
							writer = new PrintWriter(file.getAbsolutePath(), "UTF-8");
							writer.println(previewPane.getText());
							if(!previewPane.getText().endsWith("</level>")){
								writer.println("\n</level>"); 
							}
							writer.close();
						}
					} catch (FileNotFoundException e1) {
						e1.printStackTrace();
					} catch (UnsupportedEncodingException e1) {
						e1.printStackTrace();
					}	
				}
			}			
		});
		
		loadBtn.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent e) {
				int ret = fileopen.showDialog(rootPane, "Open File");
				
				if(ret == JFileChooser.APPROVE_OPTION){
					File file = fileopen.getSelectedFile();
					String text = readFile(file);
					previewPane.setText(text);
				}
			}
			
		});
		
		buttonPanel.add(addBtn);
		buttonPanel.add(clearBtn);
		buttonPanel.add(saveBtn);
		buttonPanel.add(loadBtn);
		
		mainPanel.add(buttonPanel);
		
		//Add Main Panel
		windowPanel.add(mainPanel);
		windowPanel.add(scroll);
		
		//Add Window Panel
		add(windowPanel);
		
	}

	public static void main(String[] args) {
	        
	        SwingUtilities.invokeLater(new Runnable() {
	            @Override
	            public void run() {
	            	id = JOptionPane.showInputDialog(null, "Enter Level ID:", "", 1);
	                GUI gui = new GUI();
	                gui.setVisible(true);
	            }
	        });
	}
}
