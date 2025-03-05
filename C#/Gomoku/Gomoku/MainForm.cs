/*
 *   Gomoku
 *   
 *   A game when you are in the first role, black chesses are forbidden to
 *   win from three-three, four-four, and six or more in a row. In every
 *   single suspected survive three row, we may consider any reference points
 *   forbidden by any subsequents with three-three, four-four, five in a row,
 *   or six or more in a row.
 *   
 *   Copyright (C) 2022  Edmond Chow
 *   
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Affero General Public License as published
 *   by the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *   
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Affero General Public License for more details.
 *   
 *   You should have received a copy of the GNU Affero General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *   
 *   If you have any inquiry, feel free to contact <edmond-chow@outlook.com>.
 */
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Collections;
using System.Windows.Forms;
namespace Gomoku
{
    public partial class MainForm : Form
    {
        #region helper-classes
        public class Params
        {
            private const int DefBp = 80;
            private const int DefLw = 3;
            private const int DefGs = 40;
            private const float DefCSiz = 0.75F;
            private const float DefCTou = 0.9F;
            private const float DefCSha = 1.125F;
            private const float DefCMar = 0.375F;
            public const string Game = "Gomoku";
            public const string Win = "Gomoku - Black Win";
            public const string Lost = "Gomoku - White Win";
            public const string Tied = "Gomoku - Tied";
            public const string Reset = "Reset";
            public static readonly Color BoardColor;
            public static readonly Color LineColor;
            public static readonly Color ShadowColor;
            public static readonly Color BlackChessLightColor;
            public static readonly Color BlackChessDarkColor;
            public static readonly Color WhiteChessLightColor;
            public static readonly Color WhiteChessDarkColor;
            static Params()
            {
                unchecked
                {
                    BoardColor = Color.FromArgb((int)0xFFF5DEB3u);
                    LineColor = Color.FromArgb((int)0xFFDEB887u);
                    ShadowColor = Color.FromArgb((int)0x10000000u);
                    BlackChessLightColor = Color.FromArgb((int)0xFFA0A0A0u);
                    BlackChessDarkColor = Color.FromArgb((int)0xFF000000u);
                    WhiteChessLightColor = Color.FromArgb((int)0xFFFFFFFFu);
                    WhiteChessDarkColor = Color.FromArgb((int)0xFFF0F0F0u);
                }
            }
            private int Bp;
            private int Lw;
            private int Gs;
            private float CSiz;
            private float CTou;
            private float CSha;
            private float CMar;
            public int BoardPadding
            {
                get { return Bp; }
            }
            public int LineWeight
            {
                get { return Lw; }
            }
            public int GridSize
            {
                get { return Gs; }
            }
            public float ChessSizeScale
            {
                get { return CSiz; }
            }
            public float ChessTouchScale
            {
                get { return CTou; }
            }
            public float ChessShadowScale
            {
                get { return CSha; }
            }
            public float ChessMarginScale
            {
                get { return CMar; }
            }
            public Params()
            {
                Bp = DefBp;
                Lw = DefLw;
                Gs = DefGs;
                CSiz = DefCSiz;
                CTou = DefCTou;
                CSha = DefCSha;
                CMar = DefCMar;
            }
            public bool SetParams(int BoardPadding, int LineWeight, int GridSize)
            {
                if (LineWeight >= GridSize) { return false; }
                else if (GridSize >= BoardPadding) { return false; }
                else if (BoardPadding <= 0) { return false; }
                Bp = BoardPadding;
                Lw = LineWeight;
                Gs = GridSize;
                return true;
            }
            public bool SetScales(float ChessSizeScale, float ChessTouchScale, float ChessShadowScale, float ChessMarginScale)
            {
                if (ChessSizeScale <= 0 || ChessSizeScale > 1) { return false; }
                else if (ChessTouchScale <= 0 || ChessTouchScale > 1) { return false; }
                else if (ChessShadowScale <= 0 || ChessShadowScale > 1) { return false ; }
                else if (ChessMarginScale <= 0 || ChessMarginScale > 1) { return false; }
                CSiz = ChessSizeScale;
                CTou = ChessTouchScale;
                CSha = ChessShadowScale;
                CMar = ChessMarginScale;
                return true;
            }
            public int BoardOuterSize
            {
                get { return Bp * 2 + Lw * 15 + Gs * 14; }
            }
            public int BoardInnerSize
            {
                get { return Lw * 15 + Gs * 14; }
            }
            private int GsScaleToLw(float C)
            {
                int Result = (int)(Gs * C);
                return Lw % 2 == Result % 2 ? Result : Result + 1;
            }
            public int ChessSize
            {
                get { return GsScaleToLw(CSiz); }
            }
            public int ChessTouch
            {
                get { return GsScaleToLw(CTou); }
            }
            public int ChessShadow
            {
                get { return GsScaleToLw(CSha); }
            }
            public int ChessMargin
            {
                get { return GsScaleToLw(CMar); }
            }
        }
        private struct Position
        {
            public static readonly Position Null;
            private byte Coord;
            public int X
            {
                get { return Coord & 0xF; }
                set { Coord = (byte)((value & 0xF) | (Coord & 0xF0)); }
            }
            public int Y
            {
                get { return Coord >> 4; }
                set { Coord = (byte)(((value & 0xF) << 4) | (Coord & 0xF)); }
            }
            static Position()
            {
                Null = new Position(15, 15);
            }
            private Position(byte B)
            {
                Coord = B;
            }
            public Position(int X, int Y)
            {
                Coord = (byte)((X & 0xF) | ((Y & 0xF) << 4));
            }
            public static explicit operator byte(Position Po)
            {
                return Po.Coord;
            }
            public static explicit operator Position(byte B)
            {
                return new Position(B);
            }
            public static bool operator ==(Position L, Position R)
            {
                return L.Coord == R.Coord;
            }
            public static bool operator !=(Position L, Position R)
            {
                return L.Coord != R.Coord;
            }
            public override bool Equals(object obj)
            {
                return base.Equals(obj);
            }
            public override int GetHashCode()
            {
                return base.GetHashCode();
            }
        }
        private enum Result
        {
            None = 0,
            Won = 1,
            Lost = 2,
            Tied = 3,
        }
        private enum Chess
        {
            None = 0,
            Black = 1,
            White = 2,
            Unspecified = 3,
        }
        private enum Player
        {
            Unknown = 0,
            Attacker = 1,
            Defender = 2,
            Empty = 3,
        }
        private enum Orientation
        {
            Horizontal = 0,
            Vertical = 1,
            Downward = 2,
            Upward = 3,
        }
        private struct Board : IEnumerable
        {
            private uint[] Grid;
            private byte Round;
            public void Reset()
            {
                Grid = new uint[15];
                Round = 0;
            }
            public Chess this[Position Po]
            {
                get
                {
                    if (Po.X == 15 || Po.Y == 15) { return Chess.Unspecified; }
                    int Shift = Po.X * 2;
                    return (Chess)((Grid[Po.Y] & (0b11u << Shift)) >> Shift);
                }
                set
                {
                    if (Po.X == 15 || Po.Y == 15) { return; }
                    int Shift = Po.X * 2;
                    Grid[Po.Y] = (((uint)value & 0b11u) << Shift) | (Grid[Po.Y] & ~(0b11u << Shift));
                }
            }
            public readonly struct ChessRef
            {
                private readonly uint[] Grid;
                private readonly Position Po;
                public ChessRef(uint[] Grid, Position Po)
                {
                    this.Grid = Grid;
                    this.Po = Po;
                }
                public static implicit operator Chess(ChessRef ChessRef)
                {
                    if (ChessRef.Po.X == 15 || ChessRef.Po.Y == 15) { return Chess.Unspecified; }
                    int Shift = ChessRef.Po.X * 2;
                    return (Chess)((ChessRef.Grid[ChessRef.Po.Y] & (0b11u << Shift)) >> Shift);
                }
                public Position Pos
                {
                    get { return Po; }
                }
            }
            public struct ChessIte : IEnumerator
            {
                private readonly uint[] Grid;
                private int Index;
                public ChessIte(uint[] Grid)
                {
                    this.Grid = Grid;
                    Index = -1;
                }
                public object Current
                {
                    get
                    {
                        return new ChessRef(Grid, new Position(Index % 15, Index / 15));
                    }
                }
                public bool MoveNext()
                {
                    if (Index >= 224) { return false; }
                    ++Index;
                    return true;
                }
                public void Reset()
                {
                    Index = -1;
                }
            }
            public IEnumerator GetEnumerator()
            {
                return new ChessIte(Grid);
            }
            public uint GetLine(Position Po, Orientation Or)
            {
                uint Result = 0u;
                int ShiftX = Po.X * 2;
                if (Or == Orientation.Horizontal)
                {
                    Result = Grid[Po.Y];
                    if (Po.X < 4)
                    {
                        ShiftX = 8 - ShiftX;
                        Result <<= ShiftX;
                        uint Mask = ~0u;
                        Mask <<= ShiftX;
                        Result |= ~Mask;
                    }
                    else { Result >>= ShiftX - 8; }
                }
                else
                {
                    int ShiftR = 0;
                    int Step = 0;
                    if (Or == Orientation.Downward)
                    {
                        ShiftX -= 8;
                        Step = 2;
                    }
                    else if (Or == Orientation.Upward)
                    {
                        ShiftX += 8;
                        Step = -2;
                    }
                    for (int Y = Po.Y - 4, E = Po.Y + 4; Y <= E; ++Y)
                    {
                        if (Y < 0 || Y > 14 || ShiftX < 0 || ShiftX > 28) { Result |= 0b11u << ShiftR; }
                        else { Result |= ((Grid[Y] >> ShiftX) & 0b11u) << ShiftR; }
                        ShiftX += Step;
                        ShiftR += 2;
                    }
                }
                Result |= (uint)Or << 30;
                Result |= 0x3FFC0000u;
                return Result;
            }
            public static Board operator ++(Board Board)
            {
                if (Board.Round < 225) { ++Board.Round; }
                return Board;
            }
            public bool IsBlackTurn
            {
                get { return Round % 2 == 0; }
            }
            public bool IsPending
            {
                get { return Round == 0; }
            }
            public bool IsEnded
            {
                get { return Round == 225; }
            }
        }
        private struct Counter
        {
            public readonly struct Pack
            {
                public readonly uint Mask;
                public readonly uint Case;
                public Pack(uint Mask, uint Case)
                {
                    this.Mask = Mask;
                    this.Case = Case;
                }
            }
            public readonly struct Group
            {
                public readonly int O0;
                public readonly int O1;
                public readonly int O2;
                public readonly int O3;
                public Group(int O0, int O1, int O2, int O3)
                {
                    this.O0 = O0;
                    this.O1 = O1;
                    this.O2 = O2;
                    this.O3 = O3;
                }
            }
            public struct Forbids
            {
                private uint Po;
                public Position P0
                {
                    get { return (Position)(Po & 0xFFu); }
                    set { Po = (uint)value | (Po & 0xFFFFFF00u); }
                }
                public Position P1
                {
                    get { return (Position)((Po & 0xFF00u) >> 8); }
                    set { Po = ((uint)value << 8) | (Po & 0xFFFF00FFu); }
                }
                public Position P2
                {
                    get { return (Position)((Po & 0xFF0000u) >> 16); }
                    set { Po = ((uint)value << 16) | (Po & 0xFF00FFFFu); }
                }
                public Position P3
                {
                    get { return (Position)((Po & 0xFF000000u) >> 24); }
                    set { Po = ((uint)value << 24) | (Po & 0xFFFFFFu); }
                }
                public int this[int i]
                {
                    get
                    {
                        int Shift = i * 4;
                        return (int)((Po & (0xFu << Shift)) >> Shift);
                    }
                    set
                    {
                        int Shift = i * 4;
                        Po = (((uint)value & 0xFu) << Shift) | (Po & ~(0xFu << Shift));
                    }
                }
                public Forbids(Position Po)
                {
                    this.Po = (uint)Po | ((uint)Po << 8) | ((uint)Po << 16) | ((uint)Po << 24);
                }
            }
            public static readonly Group[] B3;
            public static readonly Pack[] D4;
            public static readonly Pack[] T3;
            public static readonly Pack[] T4;
            public static readonly Pack[] T5;
            public static readonly Pack[] T6;
            public readonly Position Po;
            public uint C3;
            public uint C4;
            public uint C5;
            public uint C6;
            static Counter()
            {
                B3 = new Group[] {
                    new Group(-3, -4, 1, 2),
                    new Group(-3, -4, 1, 2),
                    new Group(-2, -3, 2, 3),
                    new Group(-2, -3, 2, 3),
                    new Group(-1, -2, 3, 4),
                    new Group(-1, -2, 3, 4),
                    new Group(0, -2, -4, 1),
                    new Group(0, 1, -2, 3),
                    new Group(0, -1, -3, 2),
                    new Group(0, 2, -1, 4),
                    new Group(0, -1, -4, 1),
                    new Group(0, 1, -1, 4),
                };
                D4 = new Pack[] {
                    new Pack(0b111111_111111_111111u, 0b010101_110111_010101u),
                    new Pack(0b001111_111111_111111u, 0b000101_110101_110101u),
                    new Pack(0b111111_111111_111100u, 0b010111_010111_010100u),
                    new Pack(0b000011_111111_111111u, 0b000001_110101_011101u),
                    new Pack(0b001111_111111_111100u, 0b000111_010101_110100u),
                    new Pack(0b111111_111111_110000u, 0b011101_010111_010000u),
                };
                T3 = new Pack[] {
                    new Pack(0b000000_111111_111111u, 0b000000_110101_011111u),
                    new Pack(0b000011_111111_111100u, 0b000011_110101_011100u),
                    new Pack(0b000011_111111_111100u, 0b000011_010101_111100u),
                    new Pack(0b001111_111111_110000u, 0b001111_010101_110000u),
                    new Pack(0b001111_111111_110000u, 0b001101_010111_110000u),
                    new Pack(0b111111_111111_000000u, 0b111101_010111_000000u),
                    new Pack(0b000000_111111_111111u, 0b000000_110101_110111u),
                    new Pack(0b001111_111111_110000u, 0b001101_110101_110000u),
                    new Pack(0b000011_111111_111100u, 0b000011_010111_011100u),
                    new Pack(0b111111_111111_000000u, 0b110111_010111_000000u),
                    new Pack(0b000000_111111_111111u, 0b000000_110111_010111u),
                    new Pack(0b111111_111111_000000u, 0b110101_110111_000000u),
                };
                T4 = new Pack[] {
                    new Pack(0b000000_001111_111111u, 0b000000_000101_010111u),
                    new Pack(0b000000_111111_111100u, 0b000000_110101_010100u),
                    new Pack(0b000000_111111_111100u, 0b000000_010101_011100u),
                    new Pack(0b000011_111111_110000u, 0b000011_010101_010000u),
                    new Pack(0b000011_111111_110000u, 0b000001_010101_110000u),
                    new Pack(0b001111_111111_000000u, 0b001101_010101_000000u),
                    new Pack(0b001111_111111_000000u, 0b000101_010111_000000u),
                    new Pack(0b111111_111100_000000u, 0b110101_010100_000000u),
                    new Pack(0b000000_001111_111111u, 0b000000_000101_011101u),
                    new Pack(0b000011_111111_110000u, 0b000001_110101_010000u),
                    new Pack(0b000000_111111_111100u, 0b000000_010101_110100u),
                    new Pack(0b001111_111111_000000u, 0b000111_010101_000000u),
                    new Pack(0b000011_111111_110000u, 0b000001_010111_010000u),
                    new Pack(0b111111_111100_000000u, 0b011101_010100_000000u),
                    new Pack(0b000000_001111_111111u, 0b000000_000101_110101u),
                    new Pack(0b001111_111111_000000u, 0b000101_110101_000000u),
                    new Pack(0b000000_111111_111100u, 0b000000_010111_010100u),
                    new Pack(0b111111_111100_000000u, 0b010111_010100_000000u),
                    new Pack(0b000000_001111_111111u, 0b000000_000111_010101u),
                    new Pack(0b111111_111100_000000u, 0b010101_110100_000000u),
                };
                T5 = new Pack[] {
                    new Pack(0b000000_001111_111111u, 0b000000_000101_010101u),
                    new Pack(0b000000_111111_111100u, 0b000000_010101_010100u),
                    new Pack(0b000011_111111_110000u, 0b000001_010101_010000u),
                    new Pack(0b001111_111111_000000u, 0b000101_010101_000000u),
                    new Pack(0b111111_111100_000000u, 0b010101_010100_000000u),
                };
                T6 = new Pack[] {
                    new Pack(0b000000_111111_111111u, 0b000000_010101_010101u),
                    new Pack(0b000011_111111_111100u, 0b000001_010101_010100u),
                    new Pack(0b001111_111111_110000u, 0b000101_010101_010000u),
                    new Pack(0b111111_111111_000000u, 0b010101_010101_000000u),
                };
            }
            public Counter(Position CurrentPo)
            {
                Po = CurrentPo;
                C3 = 0u;
                C4 = 0u;
                C5 = 0u;
                C6 = 0u;
            }
            public static uint MySide(uint Li, bool Bk)
            {
                uint Result = 0;
                for (int Shift = 0; Shift < 18; Shift += 2)
                {
                    uint Temp = (Li >> Shift) & 0b11u;
                    if (Temp == (uint)Chess.None) { Temp = (uint)Player.Empty; }
                    else if (Temp == (uint)Chess.Unspecified) { Temp = (uint)Player.Unknown; }
                    else if (Bk)
                    {
                        if (Temp == (uint)Chess.Black) { Temp = (uint)Player.Attacker; }
                        else if (Temp == (uint)Chess.White) { Temp = (uint)Player.Defender; }
                    }
                    else
                    {
                        if (Temp == (uint)Chess.Black) { Temp = (uint)Player.Defender; }
                        else if (Temp == (uint)Chess.White) { Temp = (uint)Player.Attacker; }
                    }
                    Result |= Temp << Shift;
                }
                Result |= Li & 0xC0000000u;
                return Result;
            }
            public static bool Exists(uint MySide, Pack[] Cases)
            {
                foreach (Pack P in Cases)
                {
                    if ((MySide & P.Mask) == P.Case)
                    {
                        return true;
                    }
                }
                return false;
            }
        }
        #endregion
        #region fields
        private readonly Params Pa;
        private Position Po;
        private Result Re;
        private Board Bo;
        private Graphics BoardPaint
        {
            get { return CreateGraphics(); }
        }
        private MenuItem BtnReset;
        bool Dragging;
        #endregion
        #region constructors-and-methods
        public MainForm()
        {
            Pa = new Params();
            Constructor();
        }
        public MainForm(Params Params)
        {
            Pa = Params;
            Constructor();
        }
        private void Constructor()
        {
            Menu = new MainMenu();
            BtnReset = Menu.MenuItems.Add(Params.Reset);
            BtnReset.Click += new EventHandler(ResetMenu_Click);
            InitializeComponent();
            ClientSize = new Size(Pa.BoardOuterSize, Pa.BoardOuterSize);
        }
        private void NewGame()
        {
            Po = Position.Null;
            Re = Result.None;
            Bo.Reset();
            Text = Params.Game;
            BtnReset.Enabled = false;
            Dragging = false;
        }
        private Position GetNearPos(Point Pt)
        {
            int Begin = Pa.BoardPadding - Pa.GridSize / 2 - Pa.LineWeight / 2;
            int End = Pa.BoardPadding + Pa.BoardInnerSize + Pa.GridSize / 2 + Pa.LineWeight / 2;
            int Step = Pa.GridSize + Pa.LineWeight;
            if (Pt.X < Begin || Pt.Y < Begin) { return Position.Null; }
            else if (Pt.X > End || Pt.Y > End) { return Position.Null; }
            int X = (Pt.X - Begin) / Step;
            int Y = (Pt.Y - Begin) / Step;
            if (X < 0 || X > 14 || Y < 0 || Y > 14) { return Position.Null; }
            return new Position(X, Y);
        }
        private Point GetPoint(Position Po)
        {
            int Begin = Pa.BoardPadding + Pa.LineWeight / 2;
            int Step = Pa.GridSize + Pa.LineWeight;
            int X = Begin + Step * Po.X;
            int Y = Begin + Step * Po.Y;
            return new Point(X, Y);
        }
        private double GetRadius(Position Po, Point Pt)
        {
            Point PoPt = GetPoint(Po);
            int SqX = PoPt.X - Pt.X;
            int SqY = PoPt.Y - Pt.Y;
            SqX *= SqX;
            SqY *= SqY;
            return Math.Sqrt(SqX + SqY);
        }
        private bool CanPutChess(Position Po)
        {
            if (Po == Position.Null) { return false; }
            if (Re != Result.None) { return false; }
            if (Bo.IsEnded) { return false; }
            if (Bo.IsPending && !(Po.X == 7 && Po.Y == 7)) { return false; }
            return Bo[Po] == Chess.None;
        }
        private bool CanTouchChess(Position Po, Point Pt)
        {
            int CTou = Pa.ChessTouch;
            return GetRadius(Po, Pt) <= CTou / 2d;
        }
        #endregion
        #region illustrators
        private Rectangle GetChessRect(Point Pt)
        {
            int CSiz = Pa.ChessSize;
            if (CSiz % 2 != Pa.LineWeight % 2) { ++CSiz; }
            Point Po = new Point(Pt.X - CSiz / 2 - 1, Pt.Y - CSiz / 2 - 1);
            Size Sz = new Size(CSiz + 1, CSiz + 1);
            return new Rectangle(Po, Sz);
        }
        public void PaintChess(Graphics Gr, Point Pt, bool Bk)
        {
            int CMar = Pa.ChessMargin;
            int CSha = Pa.ChessShadow;
            Rectangle CRect = GetChessRect(Pt);
            Point SPo = new Point(CRect.X - CMar, CRect.Y - CMar);
            Size SSz = new Size(CSha, CSha);
            Rectangle SRect = new Rectangle(SPo, SSz);
            GraphicsPath GP = new GraphicsPath();
            GP.AddEllipse(SRect);
            PathGradientBrush PGB = new PathGradientBrush(GP);
            if (Bk)
            {
                Gr.FillEllipse(new SolidBrush(Params.BlackChessDarkColor), CRect);
                PGB.CenterColor = Params.BlackChessLightColor;
                PGB.SurroundColors = new Color[] { Params.BlackChessDarkColor };
                Gr.FillEllipse(PGB, CRect);
            }
            else
            {
                Gr.FillEllipse(new SolidBrush(Params.WhiteChessDarkColor), CRect);
                PGB.CenterColor = Params.WhiteChessLightColor;
                PGB.SurroundColors = new Color[] { Params.WhiteChessDarkColor };
                Gr.FillEllipse(PGB, CRect);
            }
        }
        public void PaintShadow(Graphics Gr, Point Pt)
        {
            Rectangle CRect = GetChessRect(Pt);
            Gr.FillEllipse(new SolidBrush(Params.ShadowColor), CRect);
        }
        public void ClearGrid(Graphics Gr, Point Pt)
        {
            Rectangle CRect = GetChessRect(Pt);
            Gr.FillEllipse(new SolidBrush(Params.BoardColor), CRect);
            Pen LinePen = new Pen(new SolidBrush(Params.LineColor), Pa.LineWeight);
            Point Left = new Point(Po.X == 0 ? Pt.X : CRect.Left, Pt.Y);
            Point Right = new Point(Po.X == 14 ? Pt.X : CRect.Right, Pt.Y);
            Point Top = new Point(Pt.X, Po.Y == 0 ? Pt.Y : CRect.Top);
            Point Bottom = new Point(Pt.X, Po.Y == 14 ? Pt.Y : CRect.Bottom);
            Gr.DrawLine(LinePen, Left, Right);
            Gr.DrawLine(LinePen, Top, Bottom);
        }
        public void RenderClient(Graphics Gr)
        {
            Gr.Clear(Params.BoardColor);
            int Begin = Pa.BoardPadding + Pa.LineWeight / 2;
            int End = Pa.BoardPadding + Pa.BoardInnerSize - Pa.LineWeight / 2;
            int Size = Pa.BoardInnerSize - Pa.LineWeight;
            int Step = Pa.GridSize + Pa.LineWeight;
            Pen LinePen = new Pen(new SolidBrush(Params.LineColor), Pa.LineWeight);
            Gr.DrawRectangle(LinePen, new Rectangle(new Point(Begin, Begin), new Size(Size, Size)));
            for (int i = 0; i < 15; ++i)
            {
                int Adjusted = Begin + Step * i;
                Gr.DrawLine(LinePen, new Point(Begin, Adjusted), new Point(End, Adjusted));
                Gr.DrawLine(LinePen, new Point(Adjusted, Begin), new Point(Adjusted, End));
            }
            foreach (Board.ChessRef Ch in Bo)
            {
                if (Ch == Chess.Black) { PaintChess(Gr, GetPoint(Ch.Pos), true); }
                else if (Ch == Chess.White) { PaintChess(Gr, GetPoint(Ch.Pos), false); }
            }
        }
        #endregion
        #region referees
        private Counter.Forbids FindForbidPos(uint MySide, Position Po)
        {
            Orientation Or = (Orientation)(MySide >> 30);
            Counter.Forbids Fo = new Counter.Forbids(Po);
            for (int i = 0; i < Counter.T3.Length; ++i)
            {
                if ((MySide & Counter.T3[i].Mask) == Counter.T3[i].Case)
                {
                    if (Or == Orientation.Horizontal)
                    {
                        Fo[0] += Counter.B3[i].O0;
                        Fo[2] += Counter.B3[i].O1;
                        Fo[4] += Counter.B3[i].O2;
                        Fo[6] += Counter.B3[i].O3;
                    }
                    else if (Or == Orientation.Vertical)
                    {
                        Fo[1] += Counter.B3[i].O0;
                        Fo[3] += Counter.B3[i].O1;
                        Fo[5] += Counter.B3[i].O2;
                        Fo[7] += Counter.B3[i].O3;
                    }
                    else if (Or == Orientation.Downward)
                    {
                        Fo[0] += Counter.B3[i].O0;
                        Fo[2] += Counter.B3[i].O1;
                        Fo[4] += Counter.B3[i].O2;
                        Fo[6] += Counter.B3[i].O3;
                        Fo[1] += Counter.B3[i].O0;
                        Fo[3] += Counter.B3[i].O1;
                        Fo[5] += Counter.B3[i].O2;
                        Fo[7] += Counter.B3[i].O3;
                    }
                    else if (Or == Orientation.Upward)
                    {
                        Fo[0] -= Counter.B3[i].O0;
                        Fo[2] -= Counter.B3[i].O1;
                        Fo[4] -= Counter.B3[i].O2;
                        Fo[6] -= Counter.B3[i].O3;
                        Fo[1] += Counter.B3[i].O0;
                        Fo[3] += Counter.B3[i].O1;
                        Fo[5] += Counter.B3[i].O2;
                        Fo[7] += Counter.B3[i].O3;
                    }
                    break;
                }
            }
            return Fo;
        }
        private void CountMatches(uint Li, ref Counter Co)
        {
            uint MySide = Counter.MySide(Li, Bo.IsBlackTurn);
            if (Bo.IsBlackTurn && Counter.Exists(MySide, Counter.T6)) { ++Co.C6; }
            else if (Counter.Exists(MySide, Counter.T5)) { ++Co.C5; }
            else if (Bo.IsBlackTurn && Counter.Exists(MySide, Counter.D4)) { Co.C4 += 2; }
            else if (Bo.IsBlackTurn && Counter.Exists(MySide, Counter.T4)) { ++Co.C4; }
            else if (Bo.IsBlackTurn && Counter.Exists(MySide, Counter.T3))
            {
                Counter.Forbids Fo = FindForbidPos(MySide, Co.Po);
                if (Fo.P0 == Co.Po)
                {
                    if (!(S3Forbid(Fo.P1) || S3Forbid(Fo.P2) || S3Forbid(Fo.P3))) { ++Co.C3; }
                }
                else if (!(S3Forbid(Fo.P0) || S3Forbid(Fo.P1)) || !(S3Forbid(Fo.P2) || S3Forbid(Fo.P3))) { ++Co.C3; }
            }
        }
        private bool S3Forbid(Position Po)
        {
            Counter Co = new Counter(Po);
            if (Bo[Po] == Chess.White || Bo[Po] == Chess.Unspecified) { return true; }
            Bo[Po] = Chess.Black;
            CountMatches(Bo.GetLine(Po, Orientation.Horizontal), ref Co);
            CountMatches(Bo.GetLine(Po, Orientation.Vertical), ref Co);
            CountMatches(Bo.GetLine(Po, Orientation.Downward), ref Co);
            CountMatches(Bo.GetLine(Po, Orientation.Upward), ref Co);
            Bo[Po] = Chess.None;
            return Co.C3 >= 2u || Co.C4 >= 2u || Co.C5 > 0u || Co.C6 > 0u;
        }
        private void MakeResult()
        {
            Counter Co = new Counter(Po);
            CountMatches(Bo.GetLine(Po, Orientation.Horizontal), ref Co);
            CountMatches(Bo.GetLine(Po, Orientation.Vertical), ref Co);
            CountMatches(Bo.GetLine(Po, Orientation.Downward), ref Co);
            CountMatches(Bo.GetLine(Po, Orientation.Upward), ref Co);
            if (Bo.IsBlackTurn)
            {
                if (Co.C3 >= 2u || Co.C4 >= 2u || Co.C6 > 0u) { Re = Result.Lost; }
                else if (Co.C5 > 0u) { Re = Result.Won; }
            }
            else if (Co.C5 > 0u) { Re = Result.Lost; }
        }
        #endregion
        #region infrastructures
        private void SetShadow(Point Pt)
        {
            ReleastShadow(Pt);
            Position CurrentPo = GetNearPos(Pt);
            if (CurrentPo != Po && CanPutChess(CurrentPo) && CanTouchChess(CurrentPo, Pt))
            {
                PaintShadow(BoardPaint, GetPoint(CurrentPo));
                Po = CurrentPo;
            }
        }
        private void ReleastShadow(Point Pt)
        {
            if (CanPutChess(Po) && !CanTouchChess(Po, Pt))
            {
                ClearGrid(BoardPaint, GetPoint(Po));
                Po = Position.Null;
            }
        }
        private void PutChess()
        {
            BtnReset.Enabled = true;
            if (Bo.IsBlackTurn) { Bo[Po] = Chess.Black; }
            else { Bo[Po] = Chess.White; }
            PaintChess(BoardPaint, GetPoint(Po), Bo.IsBlackTurn);
            MakeResult();
            if (Re == Result.Won) { Text = Params.Win; }
            else if (Re == Result.Lost) { Text = Params.Lost; }
            else if (Re == Result.Tied) { Text = Params.Tied; }
            Po = Position.Null;
            ++Bo;
        }
        #endregion
        #region event-handlers
        private void MainForm_MouseDown(object sender, MouseEventArgs e)
        {
            Dragging = true;
        }
        private void MainForm_MouseMove(object sender, MouseEventArgs e)
        {
            if (!Dragging)
            {
                SetShadow(e.Location);
            }
        }
        private void MainForm_MouseUp(object sender, MouseEventArgs e)
        {
            Position CurrentPo = GetNearPos(e.Location);
            if (CurrentPo == Po && CanPutChess(CurrentPo) && CanTouchChess(CurrentPo, e.Location))
            {
                PutChess();
            }
            Dragging = false;
        }
        private void ResetMenu_Click(object sender, EventArgs e)
        {
            NewGame();
            RenderClient(BoardPaint);
        }
        private void MainForm_Load(object sender, EventArgs e)
        {
            NewGame();
        }
        private void MainForm_Paint(object sender, PaintEventArgs e)
        {
            RenderClient(e.Graphics);
        }
        #endregion
    }
}
