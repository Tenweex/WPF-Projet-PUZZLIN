using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Projet_Puzzle.Models
{
    internal class WrapperToImageProcessingDLL
    {
        #region Fields
        private IntPtr _clPtr;
        #endregion

        #region Constructor
        public WrapperToImageProcessingDLL()
        {
            _clPtr = IntPtr.Zero;
            Dll_Constructor();
        }
        #endregion


        #region DllImports
        [DllImport("ImageProcessing_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Build_CLibImageProcessing();
        private void Dll_Constructor()
        {
            _clPtr = Build_CLibImageProcessing();
        }





        [DllImport("ImageProcessing_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void ProcessImg(IntPtr libImageProcessing, IntPtr data, int stride, int nbLig, int nbCol);
        public void Dll_ProcessImg(IntPtr data, int stride, int nbLig, int nbCol)
        {
            ProcessImg(_clPtr, data, stride, nbLig, nbCol);
        }

        // Puzzle
        [DllImport("ImageProcessing_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void PuzzleImageProcessing(IntPtr libImageProcessing, IntPtr data, int stride, int nbLig, int nbCol);
        public void Dll_PuzzleImageProcessing(IntPtr data, int stride, int nbLig, int nbCol)
        {
            PuzzleImageProcessing(_clPtr, data, stride, nbLig, nbCol);
        }

        [DllImport("ImageProcessing_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetHauteurPuzzleCrop(IntPtr libImageProcessing);
        public int Dll_GetHauteurPuzzleCrop()
        {
            return GetHauteurPuzzleCrop(_clPtr);
        }

        [DllImport("ImageProcessing_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetLargeurPuzzleCrop(IntPtr libImageProcessing);
        public int Dll_GetLargeurPuzzleCrop()
        {
            return GetLargeurPuzzleCrop(_clPtr);
        }

        [DllImport("ImageProcessing_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void PuzzleCrop(IntPtr libImageProcessing, IntPtr data, int stride, int nbLig, int nbCol);
        public void Dll_PuzzleCrop(IntPtr data, int stride, int nbLig, int nbCol)
        {
            PuzzleCrop(_clPtr, data, stride, nbLig, nbCol);
        }




        // Piece
        [DllImport("ImageProcessing_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void PieceImageProcessing(IntPtr libImageProcessing, IntPtr data, int stride, int nbLig, int nbCol);
        public void Dll_PieceImageProcessing(IntPtr data, int stride, int nbLig, int nbCol)
        {
            PieceImageProcessing(_clPtr, data, stride, nbLig, nbCol);
        }

        [DllImport("ImageProcessing_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetHauteurPieceCrop(IntPtr libImageProcessing);
        public int Dll_GetHauteurPieceCrop()
        {
            return GetHauteurPieceCrop(_clPtr);
        }

        [DllImport("ImageProcessing_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetLargeurPieceCrop(IntPtr libImageProcessing);
        public int Dll_GetLargeurPieceCrop()
        {
            return GetLargeurPieceCrop(_clPtr);
        }

        [DllImport("ImageProcessing_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void PieceCrop(IntPtr libImageProcessing, IntPtr data, int stride, int nbLig, int nbCol);
        public void Dll_PieceCrop(IntPtr data, int stride, int nbLig, int nbCol)
        {
            PieceCrop(_clPtr, data, stride, nbLig, nbCol);
        }



        // Position Piece
        [DllImport("ImageProcessing_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SearchPiece(IntPtr libImageProcessing, IntPtr dataPu, IntPtr dataPi, int stridePu, int stridePi, int nbLigPu, int nbLigPi, int nbColPu, int nbColPi);
        public void Dll_SearchPiece(IntPtr dataPu, IntPtr dataPi, int stridePu, int stridePi, int nbLigPu, int nbLigPi, int nbColPu, int nbColPi)
        {
            SearchPiece(_clPtr, dataPu, dataPi, stridePu, stridePi, nbLigPu, nbLigPi, nbColPu, nbColPi);
        }
        #endregion
    }
}
