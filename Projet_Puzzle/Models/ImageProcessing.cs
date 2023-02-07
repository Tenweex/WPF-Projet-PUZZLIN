using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing.Imaging;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Projet_Puzzle.Models
{
    internal class ImageProcessing
    {
        private WrapperToImageProcessingDLL dll;

        public ImageProcessing()
        {
            dll = new WrapperToImageProcessingDLL();
        }

        public Bitmap StartProcess(string puzzlePath, string piecePath)
        {
            Bitmap bmp_puzzleCrop = null;
            Bitmap bmp_pieceCrop = null;

            var taskPuzzle = Task.Run(() => {
                #region Puzzle
                Bitmap bmp_puzzleIn = new Bitmap(Image.FromFile(puzzlePath));
                unsafe
                {
                    BitmapData bmpData_puzzleIn = bmp_puzzleIn.LockBits(new Rectangle(0, 0, bmp_puzzleIn.Width, bmp_puzzleIn.Height), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);

                    dll.Dll_PuzzleImageProcessing(bmpData_puzzleIn.Scan0, bmpData_puzzleIn.Stride, bmp_puzzleIn.Height, bmp_puzzleIn.Width);

                    bmp_puzzleIn.UnlockBits(bmpData_puzzleIn);
                }

                int puzzleHauteurCrop = dll.Dll_GetHauteurPuzzleCrop();
                int puzzleLargeurCrop = dll.Dll_GetLargeurPuzzleCrop();

                bmp_puzzleCrop = new Bitmap(puzzleLargeurCrop, puzzleHauteurCrop, PixelFormat.Format24bppRgb);
                unsafe
                {
                    BitmapData bmpData_puzzleCrop = bmp_puzzleCrop.LockBits(new Rectangle(0, 0, bmp_puzzleCrop.Width, bmp_puzzleCrop.Height), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);

                    dll.Dll_PuzzleCrop(bmpData_puzzleCrop.Scan0, bmpData_puzzleCrop.Stride, bmp_puzzleCrop.Height, bmp_puzzleCrop.Width);

                    bmp_puzzleCrop.UnlockBits(bmpData_puzzleCrop);
                }
                #endregion
            });

            var taskPiece = Task.Run(() => { 
                #region Piece
                Bitmap bmp_pieceIn = new Bitmap(Image.FromFile(piecePath));
                unsafe
                {
                    BitmapData bmpData_pieceIn = bmp_pieceIn.LockBits(new Rectangle(0, 0, bmp_pieceIn.Width, bmp_pieceIn.Height), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);

                    dll.Dll_PieceImageProcessing(bmpData_pieceIn.Scan0, bmpData_pieceIn.Stride, bmp_pieceIn.Height, bmp_pieceIn.Width);

                    bmp_pieceIn.UnlockBits(bmpData_pieceIn);
                }

                int pieceHauteurCrop = dll.Dll_GetHauteurPieceCrop();
                int pieceLargeurCrop = dll.Dll_GetLargeurPieceCrop();

                bmp_pieceCrop = new Bitmap(pieceLargeurCrop, pieceHauteurCrop, PixelFormat.Format24bppRgb);
                unsafe
                {
                    BitmapData bmpData_pieceCrop = bmp_pieceCrop.LockBits(new Rectangle(0, 0, bmp_pieceCrop.Width, bmp_pieceCrop.Height), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);

                    dll.Dll_PieceCrop(bmpData_pieceCrop.Scan0, bmpData_pieceCrop.Stride, bmp_pieceCrop.Height, bmp_pieceCrop.Width);

                    bmp_pieceCrop.UnlockBits(bmpData_pieceCrop);
                }
                #endregion
            });

            Task.WaitAll(taskPuzzle, taskPiece);
            #region PositionPiece
            unsafe
            {
                BitmapData bmpData_puzzleCrop = bmp_puzzleCrop.LockBits(new Rectangle(0, 0, bmp_puzzleCrop.Width, bmp_puzzleCrop.Height), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);
                BitmapData bmpData_pieceCrop = bmp_pieceCrop.LockBits(new Rectangle(0, 0, bmp_pieceCrop.Width, bmp_pieceCrop.Height), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);

                dll.Dll_SearchPiece(bmpData_puzzleCrop.Scan0, bmpData_pieceCrop.Scan0, bmpData_puzzleCrop.Stride, bmpData_pieceCrop.Stride, bmp_puzzleCrop.Height, bmp_pieceCrop.Height, bmp_puzzleCrop.Width, bmp_pieceCrop.Width);

                bmp_puzzleCrop.UnlockBits(bmpData_puzzleCrop);
                bmp_pieceCrop.UnlockBits(bmpData_pieceCrop);
            } 
            #endregion

            return bmp_puzzleCrop;
        }
    }
}
