package wci.ide.ideimpl;

import java.io.*;
import java.awt.*;
import javax.swing.*;
import javax.swing.filechooser.FileFilter;

/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2005</p>
 * <p>Company: NASA Ames Research Center</p>
 * @author Ronald Mak
 * @version 2.0
 */

class IDEFileChooser
    extends JFileChooser
{
    IDEFileChooser(String directoryPath, String filePath, IdeFileFilter filter,
                    boolean directories, boolean multiple)
    {
        super();
        this.setMultiSelectionEnabled(multiple);
        this.setFileSelectionMode(directories ? DIRECTORIES_ONLY
                                              : FILES_ONLY);

        if (filePath != null) {
            this.setSelectedFile(new File(filePath));
        }
        else {
            if (directoryPath == null) {
                directoryPath =
                    System.getProperties().
                     getProperty("file.separator").equals("/")
                    ? System.getProperties().getProperty("user.home")
                    : "c:\\";
            }

            this.setCurrentDirectory(new File(directoryPath));
        }

        if (filter != null) {
            this.addChoosableFileFilter(filter);
        }
    }

    IDEFileChooser(String directoryPath, String filePath, IdeFileFilter filter)
    {
        this(directoryPath, filePath, filter, false, false);
    }

    File choose(JTextField textField, Component parent)
    {
        int option = this.showOpenDialog(parent);

        if (option == JFileChooser.APPROVE_OPTION) {
            File file = this.getSelectedFile();
            textField.setText(file.getPath());

            return file;
        }
        else {
            return null;
        }
    }

    File choose(JTextField textField, Component parent, boolean multiple)
    {
        if (!multiple) {
            return choose(textField, parent);
        }

        int option = this.showOpenDialog(parent);

        if (option == JFileChooser.APPROVE_OPTION) {
            File files[] = this.getSelectedFiles();
            StringBuffer buffer = new StringBuffer();

            for (int i = 0; i < files.length; ++i) {
                if (i > 0) {
                    buffer.append(";");
                }
                buffer.append(files[i].getPath());
            }

            textField.setText(buffer.toString());
            return files[0];
        }
        else {
            return null;
        }
    }
}

class IdeFileFilter
    extends FileFilter
{
    private String extensions[];
    private String description;

    public IdeFileFilter(String extensions[], String description)
    {
        this.extensions = new String[extensions.length];
        this.description = description;

        for (int i = 0; i < extensions.length; ++i) {
            this.extensions[i] = extensions[i].toLowerCase();
        }
    }

    public boolean accept(File file)
    {
        if (file.isDirectory()) {
            return true;
        }

        String name = file.getName().toLowerCase();
        for (int i = 0; i < extensions.length; ++i) {
            if (name.endsWith(extensions[i])) {
                return true;
            }
        }

        return false;
    }

    public String getDescription()
    {
        return description;
    }
}
