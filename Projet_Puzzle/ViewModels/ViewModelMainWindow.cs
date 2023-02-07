using Projet_Puzzle.Controls;
using Projet_Puzzle.Utilities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;

namespace Projet_Puzzle.ViewModels
{
    public class ViewModelMainWindow : ViewModelBase
    {
		#region Fields
		private const double Const_MaximizeBorderThickness = 0;
		private const double Const_NormalBorderThickness = 8;

		private const double Const_CaptionHeight = 42;

		private const double Const_HeaderHeight = Const_CaptionHeight + Const_NormalBorderThickness;


		private WindowState _property_MainWindow_WindowState;
		private bool _property_ButtonMenu_IsChecked;

        private Visibility _property_FrameAccueil_Visibility;
        private Visibility _property_FramePositionPiece_Visibility;
        private Visibility _property_FrameResolutioPuzzle_Visibility;
        private Visibility _property_FrameParametres_Visibility;
        #endregion



        #region Properties
        // Window
        public WindowState Property_MainWindow_WindowState
		{
			get { return _property_MainWindow_WindowState; }
			set 
			{ 
				_property_MainWindow_WindowState = value;
				NotifyPropertyChanged(nameof(Property_MainWindow_WindowState));
				NotifyPropertyChanged(nameof(Property_GridMain_Margin));
			}
		}

		// WindowChrome
		public double Property_WindowChrome_CaptionHeight
        {
			get { return Const_CaptionHeight; }
		}

		public double Property_WindowChrome_ResizeBorderThickness
        {
			get { return Const_NormalBorderThickness; }
		}

		// GridMain
		public double Property_GridMain_Margin
		{
			get { return (Property_MainWindow_WindowState == WindowState.Maximized) ? Const_NormalBorderThickness : Const_MaximizeBorderThickness; }
		}

		public double Property_GridMain_RowDefinition1
        {
			get { return Const_HeaderHeight; }
		}

		// ButtonMenu
		public bool Property_ButtonMainMenu_IsChecked
        {
			get { return _property_ButtonMenu_IsChecked; }
			set 
			{ 
				_property_ButtonMenu_IsChecked = value;
				NotifyPropertyChanged(nameof(Property_ButtonMainMenu_IsChecked));
				//NotifyPropertyChanged(nameof(Property_GridMain_ColumnDefinition2));
			}
		}

        // Content
		public Visibility Property_FrameAccueil_Visibility
        {
			get { return _property_FrameAccueil_Visibility; }
			set 
			{ 
				_property_FrameAccueil_Visibility = value;
				NotifyPropertyChanged(nameof(Property_FrameAccueil_Visibility));
			}
		}

        public Visibility Property_FramePositionPiece_Visibility
        {
            get { return _property_FramePositionPiece_Visibility; }
            set
            {
                _property_FramePositionPiece_Visibility = value;
                NotifyPropertyChanged(nameof(Property_FramePositionPiece_Visibility));
            }
        }

        public Visibility Property_FrameResolutioPuzzle_Visibility
        {
            get { return _property_FrameResolutioPuzzle_Visibility; }
            set
            {
                _property_FrameResolutioPuzzle_Visibility = value;
                NotifyPropertyChanged(nameof(Property_FrameResolutioPuzzle_Visibility));
            }
        }

        public Visibility Property_FrameParametres_Visibility
        {
            get { return _property_FrameParametres_Visibility; }
            set
            {
                _property_FrameParametres_Visibility = value;
                NotifyPropertyChanged(nameof(Property_FrameParametres_Visibility));
            }
        }
        #endregion



        #region Commands
        public RelayCommand Command_ButtonsNavigation_IsChecked { get; private set; }
		#endregion



		#region Constructor
		public ViewModelMainWindow()
        {
            Command_ButtonsNavigation_IsChecked = new RelayCommand(ButtonsNavigation_IsChecked);

			Property_MainWindow_WindowState = WindowState.Normal;
            Property_ButtonMainMenu_IsChecked = true;
        }

        #endregion



        #region Events
        private void ButtonsNavigation_IsChecked(object obj)
        {
            UC_SidePanelButton button = (UC_SidePanelButton)obj;
			if (button != null)
			{
				switch (button.Name)
				{
					case "ButtonMenuAccueil":
						Property_FrameAccueil_Visibility = Visibility.Visible;
                        Property_FramePositionPiece_Visibility = Visibility.Hidden;
                        Property_FrameResolutioPuzzle_Visibility = Visibility.Hidden;
                        Property_FrameParametres_Visibility = Visibility.Hidden;
                        Property_ButtonMainMenu_IsChecked = true;
						break;

					case "ButtonMenu1":
                        Property_FrameAccueil_Visibility = Visibility.Hidden;
                        Property_FramePositionPiece_Visibility = Visibility.Visible;
                        Property_FrameResolutioPuzzle_Visibility = Visibility.Hidden;
                        Property_FrameParametres_Visibility = Visibility.Hidden;
                        Property_ButtonMainMenu_IsChecked = false;
						break;

					case "ButtonMenu2":
                        Property_FrameAccueil_Visibility = Visibility.Hidden;
                        Property_FramePositionPiece_Visibility = Visibility.Hidden;
                        Property_FrameResolutioPuzzle_Visibility = Visibility.Visible;
                        Property_FrameParametres_Visibility = Visibility.Hidden;
                        Property_ButtonMainMenu_IsChecked = false;
                        break;

                    case "ButtonMenuParametres":
                        Property_FrameAccueil_Visibility = Visibility.Hidden;
                        Property_FramePositionPiece_Visibility = Visibility.Hidden;
                        Property_FrameResolutioPuzzle_Visibility = Visibility.Hidden;
                        Property_FrameParametres_Visibility = Visibility.Visible;
                        Property_ButtonMainMenu_IsChecked = true;
                        break;

                    default:
						break;
				}
			}
        }
        #endregion
    }
}
