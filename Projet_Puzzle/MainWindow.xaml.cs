using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Projet_Puzzle
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        //Learn more here : https://learn.microsoft.com/en-us/windows/win32/api/dwmapi/ne-dwmapi-dwmwindowattribute
        [DllImport("dwmapi.dll")]
        private static extern int DwmSetWindowAttribute(IntPtr hwnd, int attr, int[] attrValue, int attrSize);
        private const int DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 = 19;
        private const int DWMWA_USE_IMMERSIVE_DARK_MODE = 20;
        private const int DWMWA_CAPTION_COLOR = 35;

        public MainWindow()
        {
            InitializeComponent();

            IntPtr hWnd = new WindowInteropHelper(this).EnsureHandle();

            if (Environment.OSVersion.Version >= new Version(10, 0, 22000)) // Minimum version : Win 11 Build 22000 - Custom caption color
            {
                string HexColorRGB = ((System.Windows.Media.Color)System.Windows.Application.Current.Resources["Window_Background"]).ToString().Substring(3);
                string HexColorGBR = string.Concat(HexColorRGB.AsSpan(4, 2), HexColorRGB.AsSpan(2, 2), HexColorRGB.AsSpan(0, 2));
                int intColor = int.Parse(HexColorGBR, System.Globalization.NumberStyles.HexNumber);
                int[] colorstr = new int[] { intColor }; // ResourceDictionary color
                //int[] colorstr = new int[] { 0xFF2020 }; // Fixed color
                DwmSetWindowAttribute(hWnd, DWMWA_CAPTION_COLOR, colorstr, sizeof(int)); // 4
            }
            else if (Environment.OSVersion.Version >= new Version(10, 0, 18985)) // Minimum version : Win 10 Build 18985 - Darkmode ON/OFF only
            {
                int[] useImmersiveDarkMode = new int[] { 1 }; // Darkmode ON = 1 / Darkmode OFF = 0
                DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, useImmersiveDarkMode, sizeof(int));
            }
            else if (Environment.OSVersion.Version >= new Version(10, 0, 17763)) // Minimum version : Win 10 Build 18985 - Darkmode ON/OFF only
            {
                int[] useImmersiveDarkMode = new int[] { 1 }; // Darkmode ON = 1 / Darkmode OFF = 0
                DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1, useImmersiveDarkMode, sizeof(int));
            }
        }
    }
}
