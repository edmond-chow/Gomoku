using System;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
namespace Gomoku
{
    public partial class MainForm : Form
    {
        private const byte WinConst = 5;
        private const byte BoardSize = 15;
        private static byte MaxRound
        {
            get { return (byte)Math.Pow(BoardSize, 2); }
        }
        private const string Title = "五子棋";
        private const string TitleBlackWin = "五子棋［黑方勝利］";
        private const string TitleWhiteWin = "五子棋［白方勝利］";
        private const string TitleTied = "五子棋［和局］";
        private const string ResetMenuText = "重置";
        private static readonly Color BoardWideColor;
        private static readonly Color BoardOffsetColor;
        private static readonly Color BlackChessLightColor;
        private static readonly Color BlackChessDarkColor;
        private static readonly Color WhiteChessLightColor;
        private static readonly Color WhiteChessDarkColor;
        private static readonly Color ShadowColor;
        private const int DefaultBoardOffset = 80;
        private const int DefaultBoardWide = 3;
        private const int DefaultChessPadding = 40;
        private const float DefaultChessSizeScale = 0.75F;
        private const float DefaultChessTouchScale = 0.9F;
        private const float DefaultChessShadowScale = 1.5F;
        private const float DefaultChessMarginScale = 0.5F;
        private readonly int BoardOffset;
        private readonly int BoardWide;
        private int BoardClientSize
        {
            get { return BoardOffset * 2 + BoardWide * BoardSize + ChessPadding * (BoardSize - 1); }
        }
        private int BoardInnerSize
        {
            get { return BoardWide * BoardSize + ChessPadding * (BoardSize - 1); }
        }
        private sbyte[,] Board;
        private readonly int ChessPadding;
        private readonly float ChessSizeScale;
        private int ChessActualSize
        {
            get
            {
                int ret = (int)Math.Floor(ChessPadding * ChessSizeScale);
                if (BoardWide % 2 == ret % 2) { return ret; }
                else { return --ret; }
            }
        }
        private readonly float ChessTouchScale;
        private int ChessActualTouch
        {
            get
            {
                int ret = (int)Math.Floor(ChessPadding * ChessTouchScale);
                if (BoardWide % 2 == ret % 2) { return ret; }
                else { return --ret; }
            }
        }
        private readonly float ChessShadowScale;
        private int ChessActualShadow
        {
            get { return (int)Math.Floor(ChessPadding * ChessSizeScale * ChessShadowScale); }
        }
        private readonly float ChessMarginScale;
        private int ChessActualMargin
        {
            get { return (int)Math.Floor(ChessPadding * ChessSizeScale * ChessMarginScale); }
        }
        private Graphics BoardPaint
        {
            get { return CreateGraphics(); }
        }
        private byte Round;
        private bool IsBlackTurn
        {
            get { return Round % 2 == 0; }
        }
        private MainMenu MyMainMenu;
        private MenuItem MyMenuItem;
        static MainForm()
        {
            BoardOffsetColor = Color.Wheat;
            BoardWideColor = Color.BurlyWood;
            BlackChessLightColor = SystemColors.ControlDark;
            BlackChessDarkColor = Color.Black;
            WhiteChessLightColor = Color.White;
            WhiteChessDarkColor = SystemColors.Control;
            ShadowColor = Color.FromArgb(16, 0, 0, 0);
        }
        public MainForm()
        {
            BoardOffset = DefaultBoardOffset;
            BoardWide = DefaultBoardWide;
            ChessPadding = DefaultChessPadding;
            ChessSizeScale = DefaultChessSizeScale;
            ChessTouchScale = DefaultChessTouchScale;
            ChessShadowScale = DefaultChessShadowScale;
            ChessMarginScale = DefaultChessMarginScale;
            Constructor();
        }
        public MainForm(int BoardOffset, int BoardWide, int ChessPadding, float ChessSizeScale, float ChessTouchScale, float ChessShadowScale, float ChessMarginScale)
        {
            this.BoardOffset = BoardOffset;
            this.BoardWide = BoardWide;
            this.ChessPadding = ChessPadding;
            this.ChessSizeScale = ChessSizeScale;
            this.ChessTouchScale = ChessTouchScale;
            this.ChessShadowScale = ChessShadowScale;
            this.ChessMarginScale = ChessMarginScale;
            Constructor();
        }
        private void Constructor()
        {
            MyMainMenu = new MainMenu();
            MyMenuItem = MyMainMenu.MenuItems.Add(ResetMenuText);
            MyMenuItem.Click += new EventHandler(ResetMenu_Click);
            Menu = MyMainMenu;
            InitializeComponent();
            ClientSize = new Size(BoardClientSize, BoardClientSize);
        }
        private class BoardPosition
        {
            public static readonly BoardPosition NullCapture;
            private readonly byte XCoordinate;
            private readonly byte YCoordinate;
            public byte X
            {
                get { return XCoordinate; }
            }
            public byte Y
            {
                get { return YCoordinate; }
            }
            static BoardPosition()
            {
                NullCapture = null;
            }
            public BoardPosition(byte XCoordinate, byte YCoordinate)
            {
                this.XCoordinate = XCoordinate;
                this.YCoordinate = YCoordinate;
            }
            private static bool ValueEquals(BoardPosition l, BoardPosition r)
            {
                return l.XCoordinate == r.XCoordinate && l.YCoordinate == r.YCoordinate;
            }
            public static bool operator ==(BoardPosition l, BoardPosition r)
            {
                if (l is null && r is null) { return true; }
                else if(l is null || r is null) { return false; }
                else { return ValueEquals(l, r); }
            }
            public static bool operator !=(BoardPosition l, BoardPosition r)
            {
                return !(l == r);
            }
            public override string ToString()
            {
                return base.ToString();
            }
            public override bool Equals(object obj)
            {
                if (obj is BoardPosition) { return this == obj as BoardPosition; }
                return base.Equals(obj);
            }
            public override int GetHashCode()
            {
                return base.GetHashCode();
            }
        }
        private enum BoardResult
        {
            Won = 1,
            Lost = -1,
            Tied = 0,
            None = 2,
        }
        private void Reset()
        {
            CaptureBP = BoardPosition.NullCapture;
            CaptureHold = false;
            Text = Title;
            MyMenuItem.Enabled = false;
            Board = new sbyte[BoardSize, BoardSize];
            Round = 0;
        }
        private void Draw(Graphics Paint)
        {
            Paint.Clear(BoardOffsetColor);
            int ResetOffset = BoardOffset + BoardWide / 2;
            int ResetDistance = BoardWide + ChessPadding;
            Paint.DrawRectangle(new Pen(new SolidBrush(BoardWideColor), BoardWide), new Rectangle(new Point(ResetOffset, ResetOffset), new Size(BoardInnerSize - BoardWide, BoardInnerSize - BoardWide)));
            for (int i = 0; i < BoardSize; ++i)
            {
                Paint.DrawLine(new Pen(new SolidBrush(BoardWideColor), BoardWide), new Point(ResetOffset + ResetDistance * i, ResetOffset), new Point(ResetOffset + ResetDistance * i, BoardOffset + BoardInnerSize));
                Paint.DrawLine(new Pen(new SolidBrush(BoardWideColor), BoardWide), new Point(ResetOffset, ResetOffset + ResetDistance * i), new Point(BoardOffset + BoardInnerSize, ResetOffset + ResetDistance * i));
            }
            for (byte i = 0; i < BoardSize; ++i)
            {
                for (byte j = 0; j < BoardSize; ++j)
                {
                    switch (Board[i, j])
                    {
                        case 1:
                            PaintChess(new BoardPosition(i, j), true);
                            break;
                        case -1:
                            PaintChess(new BoardPosition(i, j), false);
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        private BoardPosition GetNearestPosition(Point Touch)
        {
            int NearestOffset = BoardOffset - ChessPadding / 2;
            int NearestWidth = ChessPadding + BoardWide;
            int NearestX = (Touch.X - NearestOffset + NearestWidth) / NearestWidth - 1;
            if (NearestX < 0 || NearestX > BoardSize - 1) { return BoardPosition.NullCapture; }
            int NearestY = (Touch.Y - NearestOffset + NearestWidth) / NearestWidth - 1;
            if (NearestY < 0 || NearestY > BoardSize - 1) { return BoardPosition.NullCapture; }
            return new BoardPosition((byte)NearestX, (byte)NearestY);
        }
        private Point GetPoint(BoardPosition BP)
        {
            return new Point(BoardOffset + (BoardWide + ChessPadding) * BP.X + BoardWide / 2, BoardOffset + (BoardWide + ChessPadding) * BP.Y + BoardWide / 2);
        }
        private bool CanPutChess(Point Touch)
        {
            if (Round >= MaxRound) { return false; }
            BoardPosition Nearest = GetNearestPosition(Touch);
            if (Nearest == BoardPosition.NullCapture) { return false; }
            Point NearestPoint = GetPoint(Nearest);
            if (Math.Floor(Math.Sqrt(Math.Pow(NearestPoint.X - Touch.X, 2) + Math.Pow(NearestPoint.Y - Touch.Y, 2))) > (float)ChessActualTouch / 2) { return false; }
            if (Round == 0 && !(Nearest.X == 7 && Nearest.Y == 7)) { return false; }
            if (Board[Nearest.X, Nearest.Y] != 0) { return false; }
            return true;
        }
        private void PaintChess(BoardPosition Nearest, bool IsBlackChess)
        {
            Point NearestPoint = GetPoint(Nearest);
            Rectangle ChessOriginRect = new Rectangle(new Point(NearestPoint.X - BoardWide / 2 - (ChessActualSize - BoardWide) / 2 - 1, NearestPoint.Y - BoardWide / 2 - (ChessActualSize - BoardWide) / 2 - 1), new Size(ChessActualSize + 1, ChessActualSize + 1));
            Rectangle ChessGPRect = new Rectangle(new Point(ChessOriginRect.X - ChessActualMargin - 1, ChessOriginRect.Y - ChessActualMargin - 1), new Size(ChessActualShadow + 1, ChessActualShadow + 1));
            System.Drawing.Drawing2D.GraphicsPath GP = new System.Drawing.Drawing2D.GraphicsPath();
            GP.AddEllipse(ChessGPRect);
            System.Drawing.Drawing2D.PathGradientBrush PGB = new System.Drawing.Drawing2D.PathGradientBrush(GP);
            if (IsBlackChess == true)
            {
                BoardPaint.FillEllipse(new SolidBrush(BlackChessDarkColor), ChessOriginRect);
                PGB.CenterColor = BlackChessLightColor;
                PGB.SurroundColors = new Color[] { BlackChessDarkColor };
                BoardPaint.FillEllipse(PGB, ChessOriginRect);
            }
            else
            {
                BoardPaint.FillEllipse(new SolidBrush(WhiteChessDarkColor), ChessOriginRect);
                PGB.CenterColor = WhiteChessLightColor;
                PGB.SurroundColors = new Color[] { WhiteChessDarkColor };
                BoardPaint.FillEllipse(PGB, ChessOriginRect);
            }
        }
        BoardPosition CaptureBP;
        bool CaptureHold;
        private void SetShadow(Point Capture)
        {
            BoardPosition Nearest = GetNearestPosition(Capture);
            if (CaptureBP != Nearest)
            {
                ReleastShadow();
                if (CanPutChess(Capture))
                {
                    Point NearestPoint = GetPoint(Nearest);
                    Rectangle ChessNearestRect = new Rectangle(new Point(NearestPoint.X - BoardWide / 2 - (ChessActualSize - BoardWide) / 2 - 1, NearestPoint.Y - BoardWide / 2 - (ChessActualSize - BoardWide) / 2 - 1), new Size(ChessActualSize + 1, ChessActualSize + 1));
                    BoardPaint.FillEllipse(new SolidBrush(ShadowColor), ChessNearestRect);
                    CaptureBP = Nearest;
                }
            }
        }
        private void ReleastShadow()
        {
            if (CaptureBP != BoardPosition.NullCapture)
            {
                Point CapturePoint = GetPoint(CaptureBP);
                Rectangle ChessCaptureRect = new Rectangle(new Point(CapturePoint.X - BoardWide / 2 - (ChessActualSize - BoardWide) / 2 - 1, CapturePoint.Y - BoardWide / 2 - (ChessActualSize - BoardWide) / 2 - 1), new Size(ChessActualSize + 1, ChessActualSize + 1));
                BoardPaint.FillEllipse(new SolidBrush(BoardOffsetColor), ChessCaptureRect);
                BoardPaint.DrawLine(new Pen(new SolidBrush(BoardWideColor), BoardWide), new Point(CaptureBP.X == 0 ? CapturePoint.X : ChessCaptureRect.Left, CapturePoint.Y), new Point(CaptureBP.X == BoardSize - 1 ? CapturePoint.X : ChessCaptureRect.Right, CapturePoint.Y));
                BoardPaint.DrawLine(new Pen(new SolidBrush(BoardWideColor), BoardWide), new Point(CapturePoint.X, CaptureBP.Y == 0 ? CapturePoint.Y : ChessCaptureRect.Top), new Point(CapturePoint.X, CaptureBP.Y == BoardSize - 1 ? CapturePoint.Y : ChessCaptureRect.Bottom));
                CaptureBP = BoardPosition.NullCapture;
            }
        }
        private delegate bool CaseRange(int i);
        private delegate sbyte CaseBoard(int i, int j);
        private delegate bool CaseArray(int[] data);
        private delegate sbyte CheckBoard(int i, int j, int k);
        private int BlackCase6C4(CaseRange Range, CaseBoard Board, CaseArray Array)
        {
            if (Range.GetInvocationList().Length != 1 || Board.GetInvocationList().Length != 1 || Array.GetInvocationList().Length != 1) { throw new NotImplementedException(); }
            if (IsBlackTurn == false) { return 0; }
            int[] data = new int[6];
            for (int i = 0; i < 4; ++i)
            {
                if (Range(i)) { continue; }
                for (int j = 0; j < 6; ++j) { data[j] = Board(i, j); }
                if (Array(data)) { return 1; }
            }
            return 0;
        }
        private int BlackBase6C4(BoardPosition BP, CaseArray Array)
        {
            if (Array.GetInvocationList().Length != 1) { throw new NotImplementedException(); }
            if (IsBlackTurn == false) { return 0; }
            if (BP.X > BoardSize - 1 || BP.X < 0) { return 0; }
            if (BP.Y > BoardSize - 1 || BP.Y < 0) { return 0; }
            return BlackCase6C4
            (
                (int i) => { return BP.X - i - 1 < 0 || BP.X - i + 4 > BoardSize - 1; },
                (int i, int j) => { return Board[BP.X - i + j - 1, BP.Y]; }, Array
            )
            + BlackCase6C4
            (
                (int i) => { return BP.Y - i - 1 < 0 || BP.Y - i + 4 > BoardSize - 1; },
                (int i, int j) => { return Board[BP.X, BP.Y - i + j - 1]; }, Array
            )
            + BlackCase6C4
            (
                (int i) => { return BP.X - i - 1 < 0 || BP.X - i + 4 > BoardSize - 1 || BP.Y - i - 1 < 0 || BP.Y - i + 4 > BoardSize - 1; },
                (int i, int j) => { return Board[BP.X - i + j - 1, BP.Y - i + j - 1]; }, Array
            )
            + BlackCase6C4
            (
                (int i) => { return BP.X + i - 4 < 0 || BP.X + i + 1 > BoardSize - 1 || BP.Y - i - 1 < 0 || BP.Y - i + 4 > BoardSize - 1; },
                (int i, int j) => { return Board[BP.X + i - j + 1, BP.Y - i + j - 1]; }, Array
            );
        }
        private int BlackCase5C5(CaseRange Range, CaseBoard Board, CaseArray Array)
        {
            if (Range.GetInvocationList().Length != 1 || Board.GetInvocationList().Length != 1 || Array.GetInvocationList().Length != 1) { throw new NotImplementedException(); }
            if (IsBlackTurn == false) { return 0; }
            int[] data = new int[5];
            for (int i = 0; i < 5; ++i)
            {
                if (Range(i)) { continue; }
                for (int j = 0; j < 5; ++j) { data[j] = Board(i, j); }
                if (Array(data)) { return 1; }
            }
            return 0;
        }
        private int BlackBase5C5(BoardPosition BP, CaseArray Array)
        {
            if (Array.GetInvocationList().Length != 1) { throw new NotImplementedException(); }
            if (IsBlackTurn == false) { return 0; }
            if (BP.X > BoardSize - 1 || BP.X < 0) { return 0; }
            if (BP.Y > BoardSize - 1 || BP.Y < 0) { return 0; }
            return BlackCase5C5
            (
                (int i) => { return BP.X - i < 0 || BP.X - i + 4 > BoardSize - 1; },
                (int i, int j) => { return Board[BP.X - i + j, BP.Y]; }, Array
            )
            + BlackCase5C5
            (
                (int i) => { return BP.Y - i < 0 || BP.Y - i + 4 > BoardSize - 1; },
                (int i, int j) => { return Board[BP.X, BP.Y - i + j]; }, Array
            )
            + BlackCase5C5
            (
                (int i) => { return BP.X - i < 0 || BP.X - i + 4 > BoardSize - 1 || BP.Y - i < 0 || BP.Y - i + 4 > BoardSize - 1; },
                (int i, int j) => { return Board[BP.X - i + j, BP.Y - i + j]; }, Array
            )
            + BlackCase5C5
            (
                (int i) => { return BP.X + i - 4 < 0 || BP.X + i > BoardSize - 1 || BP.Y - i < 0 || BP.Y - i + 4 > BoardSize - 1; },
                (int i, int j) => { return Board[BP.X + i - j, BP.Y - i + j]; }, Array
            );
        }
        private int BlackS4(BoardPosition BP)
        {
            if (IsBlackTurn == false) { return 0; }
            return BlackBase6C4(BP, (int[] data) => { return data.SequenceEqual(new int[] { 0, 1, 1, 1, 1, 0 }); });
        }
        private int BlackD4(BoardPosition BP)
        {
            if (IsBlackTurn == false) { return 0; }
            return BlackBase6C4(BP, (int[] data) => { return data.SequenceEqual(new int[] { -1, 1, 1, 1, 1, 0 }); })
                 + BlackBase6C4(BP, (int[] data) => { return data.SequenceEqual(new int[] { 0, 1, 1, 1, 1, -1 }); })
                 + BlackBase5C5(BP, (int[] data) => { return data.SequenceEqual(new int[] { 1, 0, 1, 1, 1 }); })
                 + BlackBase5C5(BP, (int[] data) => { return data.SequenceEqual(new int[] { 1, 1, 0, 1, 1 }); })
                 + BlackBase5C5(BP, (int[] data) => { return data.SequenceEqual(new int[] { 1, 1, 1, 0, 1 }); });
        }
        private int BlackS3(BoardPosition BP)
        {
            if (IsBlackTurn == false) { return 0; }
            return BlackBase6C4(BP, (int[] data) => { return data.SequenceEqual(new int[] { 0, 0, 1, 1, 1, 0 }) || data.SequenceEqual(new int[] { 0, 1, 1, 1, 0, 0 }); })
                 + BlackBase6C4(BP, (int[] data) => { return data.SequenceEqual(new int[] { 0, 1, 0, 1, 1, 0 }) || data.SequenceEqual(new int[] { 0, 1, 1, 0, 1, 0 }); });
        }
        private BoardResult CheckForWinProto(int i, int j, CheckBoard Board, CaseBoard Black)
        {
            if (Black.GetInvocationList().Length != 1) { throw new NotImplementedException(); }
            int WinCount = 0;
            for (int k = 0; k < WinConst; ++k) { WinCount += Board(i, j, k); }
            if (WinCount == WinConst)
            {
                if (Black(i, j) == 1) { return BoardResult.Lost; }
                return BoardResult.Won;
            }
            else if (WinCount == -WinConst) { return BoardResult.Lost; }
            return BoardResult.None;
        }
        private BoardResult CheckForWinCaseFlat(CheckBoard Board, CaseBoard Black)
        {
            if (Black.GetInvocationList().Length != 1) { throw new NotImplementedException(); }
            for (int i = 0; i < BoardSize - 1; ++i)
            {
                for (int j = 0; j < BoardSize - (WinConst - 1); ++j)
                {
                    BoardResult BR = CheckForWinProto(i, j, Board, Black);
                    if (BR != BoardResult.None) { return BR; }
                }
            }
            return BoardResult.None;
        }
        private BoardResult CheckForWinCaseLean(CheckBoard Board, CaseBoard Black)
        {
            if (Black.GetInvocationList().Length != 1) { throw new NotImplementedException(); }
            for (int i = -(BoardSize - WinConst + 1); i < BoardSize - WinConst + 1; ++i)
            {
                for (int j = 0; j < BoardSize - Math.Abs(i) - WinConst + 1; ++j)
                {
                    BoardResult BR = CheckForWinProto(i, j, Board, Black);
                    if (BR != BoardResult.None) { return BR; }
                }
            }
            return BoardResult.None;
        }
        private BoardPosition PutChess(Point Touch)
        {
            BoardPosition Nearest = GetNearestPosition(Touch);
            if (IsBlackTurn == true) { Board[Nearest.X, Nearest.Y] = 1; }
            else { Board[Nearest.X, Nearest.Y] = -1; }
            PaintChess(Nearest, IsBlackTurn);
            return Nearest;
        }
        private BoardResult CheckForWin(BoardPosition BP)
        {
            if (Round > MaxRound) { return BoardResult.None; }
            if ((BlackS4(BP) + BlackD4(BP) >= 2 || BlackS3(BP) >= 2) && IsBlackTurn == true) { return BoardResult.Lost; }
            BoardResult[] Case = new BoardResult[]
            {
                CheckForWinCaseFlat((int i, int j, int k) => { return Board[i, j + k]; }, (int i, int j) => { return Board[i, j + WinConst]; }),
                CheckForWinCaseFlat((int i, int j, int k) => { return Board[j + k, i]; }, (int i, int j) => { return Board[j + WinConst, i]; }),
                CheckForWinCaseLean(
                    (int i, int j, int k) => { return Board[Math.Max(i, 0) + j + k, Math.Abs(Math.Min(i, 0)) + j + k]; },
                    (int i, int j) => { return Board[Math.Max(i, 0) + j + WinConst, Math.Abs(Math.Min(i, 0)) + j + WinConst]; }
                ),
                CheckForWinCaseLean(
                    (int i, int j, int k) => { return Board[Math.Min(BoardSize - 1 - i, BoardSize - 1) - j - k, Math.Abs(Math.Min(i, 0)) + j + k]; },
                    (int i, int j) => { return Board[Math.Min(BoardSize - 1 - i, BoardSize - 1) - j - WinConst, Math.Abs(Math.Min(i, 0)) + j + WinConst]; }
                ),
            };
            for (int i = 0; i < Case.Length; ++i)
            {
                if (Case[i] != BoardResult.None) { return Case[i]; }
            }
            if (Round == MaxRound) { return BoardResult.Tied; }
            return BoardResult.None;
        }
        private void Result(BoardResult BR)
        {
            MyMenuItem.Enabled = true;
            ++Round;
            if (BR == BoardResult.None) { return; }
            Round = (byte)(MaxRound + 1);
            if (BR == BoardResult.Won) { Text = TitleBlackWin; }
            else if (BR == BoardResult.Lost) { Text = TitleWhiteWin; }
            else { Text = TitleTied; }
        }
        private void MainForm_MouseDown(object sender, MouseEventArgs e)
        {
            CaptureHold = true;
        }
        private void MainForm_MouseMove(object sender, MouseEventArgs e)
        {
            if (CaptureHold == false) { SetShadow(e.Location); }
        }
        private void MainForm_MouseUp(object sender, MouseEventArgs e)
        {
            if (CanPutChess(e.Location) && GetNearestPosition(e.Location) == CaptureBP)
            {
                Result(CheckForWin(PutChess(e.Location)));
                CaptureBP = BoardPosition.NullCapture;
            }
            ReleastShadow();
            CaptureHold = false;
        }
        private void ResetMenu_Click(object sender, EventArgs e)
        {
            Reset();
            Draw(BoardPaint);
        }
        private void MainForm_Load(object sender, EventArgs e)
        {
            Reset();
        }
        private void MainForm_Paint(object sender, PaintEventArgs e)
        {
            Draw(e.Graphics);
        }
    }
}
