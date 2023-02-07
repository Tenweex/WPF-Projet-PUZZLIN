using Projet_Puzzle.Controls;
using Projet_Puzzle.Models;
using Projet_Puzzle.Utilities;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace Projet_Puzzle.ViewModels
{
    class ViewModelPositionPiece : ViewModelBase
    {
        #region Fields
        // SidePanel
        private Visibility _property_SidePanelAddFolderButton_Visibility;
        private Visibility _property_SidePanelPiecesList_Visibility;

        private ObservableCollection<ImageRadioButton> _property_ListOfPuzzlePieces;

        // Content
        private Visibility _property_ContentAddFileButton_Visibility;
        private Visibility _property_ContentTraitement_Visibility;

        private ImageSource _property_ImgPuzzle_ImageSource;

        // Waiting
        private Visibility _property_WaitingPanel_Visibility;


        // Others
        private string _puzzle_FilePath;
        private string _piece_FilePath;
        private ImageProcessing _imageProcessing;

        private Thread _thread;
        private AutoResetEvent _evProcess;
        private int _idThreadActiv;
        #endregion



        #region Properties
        // SidePanel
        public Visibility Property_SidePanelAddFolderButton_Visibility
        {
            get { return _property_SidePanelAddFolderButton_Visibility; }
            set
            {
                _property_SidePanelAddFolderButton_Visibility = value;
                NotifyPropertyChanged(nameof(Property_SidePanelAddFolderButton_Visibility));
            }
        }

        public Visibility Property_SidePanelPiecesList_Visibility
        {
            get { return _property_SidePanelPiecesList_Visibility; }
            set
            {
                _property_SidePanelPiecesList_Visibility = value;
                NotifyPropertyChanged(nameof(Property_SidePanelPiecesList_Visibility));
            }
        }

        public ObservableCollection<ImageRadioButton> Property_ListOfPuzzlePieces
        {
            get { return _property_ListOfPuzzlePieces; }
            set 
            {
                _property_ListOfPuzzlePieces = value;
                NotifyPropertyChanged(nameof(Property_ListOfPuzzlePieces));
            }
        }

        public string Property_SelectedPuzzlePiece_FilePath
        {
            get
            {
                string str = "";
                foreach (ImageRadioButton imageRadioButton in Property_ListOfPuzzlePieces)
                {
                    if (imageRadioButton.IsChecked)
                        str = imageRadioButton.ImageSource.ToString();
                }
                return str;
            }
        }

        // Content
        public Visibility Property_ContentAddFileButton_Visibility
        {
            get { return _property_ContentAddFileButton_Visibility; }
            set 
            {
                _property_ContentAddFileButton_Visibility = value; 
                NotifyPropertyChanged(nameof(Property_ContentAddFileButton_Visibility));
            }
        }

        public Visibility Property_ContentTraitement_Visibility
        {
            get { return _property_ContentTraitement_Visibility; }
            set
            {
                _property_ContentTraitement_Visibility = value;
                NotifyPropertyChanged(nameof(Property_ContentTraitement_Visibility));
            }
        }

        public ImageSource Property_ImgPuzzle_ImageSource
        {
            get { return _property_ImgPuzzle_ImageSource; }
            set 
            {
                _property_ImgPuzzle_ImageSource = value;
                NotifyPropertyChanged(nameof(Property_ImgPuzzle_ImageSource));
            }
        }

        // Waiting
        public Visibility Property_WaitingPanel_Visibility
        {
            get { return _property_WaitingPanel_Visibility; }
            set 
            { 
                _property_WaitingPanel_Visibility = value;
                NotifyPropertyChanged(nameof(Property_WaitingPanel_Visibility));
            }
        }
        #endregion



        #region Commands
        public RelayCommand Command_ButtonAddFolder_Click { get; private set; }
        public RelayCommand Command_ButtonAddFile_Click { get; private set; }
        public RelayCommand Command_ButtonTraitement_Click { get; private set; }
        #endregion



        #region Constructor
        public ViewModelPositionPiece()
        {
            Command_ButtonAddFolder_Click = new RelayCommand(ButtonAddFolder_Click);
            Command_ButtonAddFile_Click = new RelayCommand(ButtonAddFile_Click);
            Command_ButtonTraitement_Click = new RelayCommand(ButtonTraitement_Click);

            Property_SidePanelAddFolderButton_Visibility = Visibility.Visible;
            Property_SidePanelPiecesList_Visibility = Visibility.Collapsed;
            Property_ListOfPuzzlePieces = new ObservableCollection<ImageRadioButton>();

            Property_ContentAddFileButton_Visibility = Visibility.Visible;
            Property_ContentTraitement_Visibility= Visibility.Collapsed;

            Property_WaitingPanel_Visibility = Visibility.Collapsed;

            _puzzle_FilePath = "";
            _piece_FilePath = "";
            _imageProcessing = new ImageProcessing();

        }
        #endregion



        #region Events
        private void ButtonAddFolder_Click(object obj)
        {
            FolderBrowserDialog folderBrowserDialog = new()
            {
                UseDescriptionForTitle = true,
                Description = "Veuillez choisir le dossier contenant les pièces de puzzle"
            };

            if (folderBrowserDialog.ShowDialog() == DialogResult.OK)
            {
                string folderPath = folderBrowserDialog.SelectedPath;
                string[] bmpFiles = Directory.GetFiles(folderPath, "*.bmp");

                if (bmpFiles.Length != 0)
                {
                    Property_ListOfPuzzlePieces.Clear();
                    GC.Collect();

                    foreach (string bmpFile in bmpFiles)
                    {
                        Property_ListOfPuzzlePieces.Add(new ImageRadioButton() { ImageSource = bmpFile, GroupName = "PuzzlePieces", IsChecked = false});
                    }
                    
                    Property_SidePanelAddFolderButton_Visibility = Visibility.Collapsed;
                    Property_SidePanelPiecesList_Visibility = Visibility.Visible;
                }
                else
                {
                    System.Windows.MessageBox.Show("Aucun fichier bitmap (.bmp) n'est présent dans ce dossier !", "Aucun fichier .bmp !", MessageBoxButton.OK, MessageBoxImage.Warning);
                }
            }
        }

        private void ButtonAddFile_Click(object obj)
        {
            OpenFileDialog openFileDialog = new()
            {
                Multiselect = false,
                Filter = "Fichier bitmap (*.bmp)|*.bmp",
                Title = "Veuillez choisir le puzzle"
            };

            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                string bmpFile = openFileDialog.FileName;
                if (bmpFile != null && bmpFile.EndsWith(".bmp"))
                {
                    _puzzle_FilePath = bmpFile;

                    Bitmap bitmap = new(Image.FromFile(_puzzle_FilePath));
                    Property_ImgPuzzle_ImageSource = ImageUtilities.ImageSourceFromBitmap(bitmap);

                    Property_ContentAddFileButton_Visibility = Visibility.Collapsed;
                    Property_ContentTraitement_Visibility= Visibility.Visible;
                }
            }
        }

        private void ButtonTraitement_Click(object obj)
        {
            if (Property_SelectedPuzzlePiece_FilePath == "")
            {
                System.Windows.MessageBox.Show("Veuillez choisir une pièce de puzzle !", "Aucune pièce de puzzle sélectionnée !", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            if (_puzzle_FilePath == "")
            {
                System.Windows.MessageBox.Show("Veuillez choisir un puzzle !", "Aucun puzzle sélectionné !", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            // Lancement du traitement d'image
            Property_WaitingPanel_Visibility = Visibility.Visible;
            _piece_FilePath = Property_SelectedPuzzlePiece_FilePath;

            // Threads
            if (_evProcess != null)
            {
                _evProcess.Dispose();
                _evProcess = null;
            }
            if (_thread != null)
            {
                _thread = null;
            }



            _evProcess = new AutoResetEvent(false);
            _thread = new Thread(new ParameterizedThreadStart(process));
            _thread.Start();
        }
        #endregion



        #region Methods
        private void process(object idThread)
        {
            Bitmap bitmap = _imageProcessing.StartProcess(_puzzle_FilePath, _piece_FilePath);

            System.Windows.Application.Current.Dispatcher.Invoke(() =>
            {
                Property_ImgPuzzle_ImageSource = ImageUtilities.ImageSourceFromBitmap(bitmap);
                Property_WaitingPanel_Visibility = Visibility.Collapsed;
            });
        }
        #endregion
    }
}
