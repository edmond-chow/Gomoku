﻿#pragma region license
/*
 *   Gomoku
 *   
 *   A game when you are in the first role, Black Chesses are Forbidden to
 *   Win from three-three, four-four, and six or more in a row. In every
 *   single Suspected Survive Three row, we shall consider any reference
 *   point Forbidden by any subsequent with three-three, four-four, five in a
 *   row, or six or more in a row.
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
#pragma endregion
#include <cmath>
#include <string>
#include <Windows.h>
#include <gdiplus.h>
#include "res.h"
namespace Gomoku
{
    class MainWindow
    {
#pragma region constants
    private:
        static constexpr const std::uint32_t Byte = 0xFFu;
        static constexpr const std::uint32_t Nibble = 0xFu;
        static constexpr const std::uint32_t Box = 0b11u;
        static constexpr const std::uint32_t Bit = 0b1u;
#pragma endregion
#pragma region helper-classes
    public:
        class Params
        {
        private:
            static constexpr const int DefBp = 80;
            static constexpr const int DefLw = 3;
            static constexpr const int DefGs = 40;
            static constexpr const float DefCSiz = 0.75F;
            static constexpr const float DefCTou = 0.9F;
            static constexpr const float DefCSha = 1.125F;
            static constexpr const float DefCMar = 0.375F;
        public:
            static const Params Default;
            static constexpr const wchar_t Game[] = L"Gomoku";
            static constexpr const wchar_t Win[] = L"Gomoku - Black Win";
            static constexpr const wchar_t Lost[] = L"Gomoku - White Win";
            static constexpr const wchar_t Tied[] = L"Gomoku - Tied";
            static constexpr const wchar_t Reset[] = L"Reset";
            static const Gdiplus::Color BoardColor;
            static const Gdiplus::Color LineColor;
            static const Gdiplus::Color ShadowColor;
            static const Gdiplus::Color BlackChessLightColor;
            static const Gdiplus::Color BlackChessDarkColor;
            static const Gdiplus::Color WhiteChessLightColor;
            static const Gdiplus::Color WhiteChessDarkColor;
            static const Gdiplus::GdiplusStartupInput StartupInput;
        private:
            int Bp;
            int Lw;
            int Gs;
            float CSiz;
            float CTou;
            float CSha;
            float CMar;
        public:
            constexpr int BoardPadding() const &
            {
                return Bp;
            };
            constexpr int LineWeight() const &
            {
                return Lw;
            };
            constexpr int GridSize() const &
            {
                return Gs;
            };
            constexpr float ChessSizeScale() const &
            {
                return CSiz;
            };
            constexpr float ChessTouchScale() const &
            {
                return CTou;
            };
            constexpr float ChessShadowScale() const &
            {
                return CSha;
            };
            constexpr float ChessMarginScale() const &
            {
                return CMar;
            };
            constexpr Params()
                : Bp{ DefBp }, Lw{ DefLw }, Gs{ DefGs }, CSiz{ DefCSiz }, CTou{ DefCTou }, CSha{ DefCSha }, CMar{ DefCMar }
            {};
            constexpr bool SetParams(int BoardPadding, int LineWeight, int GridSize) &
            {
                if (LineWeight >= GridSize) { return false; }
                else if (GridSize >= BoardPadding) { return false; }
                else if (BoardPadding <= 0) { return false; }
                Bp = BoardPadding;
                Lw = LineWeight;
                Gs = GridSize;
                return true;
            };
            constexpr bool SetScales(float ChessSizeScale, float ChessTouchScale, float ChessShadowScale, float ChessMarginScale) &
            {
                if (ChessSizeScale <= 0 || ChessSizeScale > 1) { return false; }
                else if (ChessTouchScale <= 0 || ChessTouchScale > 1) { return false; }
                else if (ChessShadowScale <= 0 || ChessShadowScale > 1) { return false; }
                else if (ChessMarginScale <= 0 || ChessMarginScale > 1) { return false; }
                CSiz = ChessSizeScale;
                CTou = ChessTouchScale;
                CSha = ChessShadowScale;
                CMar = ChessMarginScale;
                return true;
            };
            constexpr int BoardOuterSize() const &
            {
                return Bp * 2 + Lw * 15 + Gs * 14;
            };
            constexpr int BoardInnerSize() const &
            {
                return Lw * 15 + Gs * 14;
            };
            constexpr int GsScaleToLw(float C) const &
            {
                int Result = static_cast<int>(Gs * C);
                return Lw % 2 == Result % 2 ? Result : Result + 1;
            };
            constexpr int ChessSize() const &
            {
                return GsScaleToLw(CSiz);
            };
            constexpr int ChessTouch() const &
            {
                return GsScaleToLw(CTou);
            };
            constexpr int ChessShadow() const &
            {
                return GsScaleToLw(CSha);
            };
            constexpr int ChessMargin() const &
            {
                return GsScaleToLw(CMar);
            };
        };
    private:
        enum struct Result : std::uint32_t
        {
            None = 0u,
            Won = 1u,
            Lost = 2u,
            Tied = 3u,
        };
        enum struct Chess : std::uint32_t
        {
            None = 0u,
            Black = 1u,
            White = 2u,
            Unspecified = 3u,
        };
        enum struct Player : std::uint32_t
        {
            Unknown = 0u,
            Attacker = 1u,
            Defender = 2u,
            Empty = 3u,
        };
        enum struct Orientation : std::uint32_t
        {
            Horizontal = 0u,
            Vertical = 1u,
            Downward = 2u,
            Upward = 3u,
        };
        struct Position
        {
        public:
            static const Position Null;
        private:
            std::uint32_t Coord;
        public:
            constexpr int X() const &
            {
                return static_cast<int>(Coord & Nibble);
            };
            constexpr void X(int value) &
            {
                Coord &= ~Nibble;
                Coord |= static_cast<std::uint32_t>(value) & Nibble;
            };
            constexpr int Y() const &
            {
                return static_cast<int>((Coord >> 4) & Nibble);
            };
            constexpr void Y(int value) &
            {
                Coord &= ~(Nibble << 4);
                Coord |= (static_cast<std::uint32_t>(value) & Nibble) << 4;
            };
            constexpr explicit Position(std::uint8_t B)
                : Coord{ B }
            {};
            constexpr Position(int X, int Y)
                : Coord{ 0u }
            {
                this->X(X);
                this->Y(Y);
            };
            constexpr explicit operator std::uint8_t()
            {
                return static_cast<std::uint8_t>(Coord & Byte);
            };
            constexpr explicit operator std::uint16_t()
            {
                return static_cast<std::uint16_t>(Coord & Byte);
            };
            constexpr explicit operator std::uint32_t()
            {
                return static_cast<std::uint32_t>(Coord & Byte);
            };
            constexpr explicit operator std::uint64_t()
            {
                return static_cast<std::uint64_t>(Coord & Byte);
            };
            friend static constexpr bool operator ==(Position L, Position R)
            {
                return L.Coord == R.Coord;
            };
            friend static constexpr bool operator !=(Position L, Position R)
            {
                return L.Coord != R.Coord;
            };
        };
        struct Board
        {
        public:
            struct ChessCRef
            {
            public:
                friend struct Board;
            private:
                const std::uint32_t* Grid;
                Position Po;
                constexpr explicit ChessCRef(const std::uint32_t* Grid, Position Po)
                    : Grid{ Grid }, Po{ Po }
                {};
            public:
                constexpr operator Chess() const
                {
                    int PoX = Po.X();
                    int PoY = Po.Y();
                    if (PoX == 15 || PoY == 15) { return Chess::Unspecified; }
                    int Shift = PoX * 2;
                    return static_cast<Chess>((Grid[PoY] >> Shift) & Box);
                };
                constexpr Position Pos() const
                {
                    return Po;
                };
            };
            struct ChessRef
            {
            public:
                friend struct Board;
            private:
                std::uint32_t* Grid;
                Position Po;
                constexpr explicit ChessRef(std::uint32_t* Grid, Position Po)
                    : Grid{ Grid }, Po{ Po }
                {};
            public:
                constexpr operator ChessCRef()
                {
                    return ChessCRef{ Grid, Po };
                };
                constexpr operator Chess() const
                {
                    int PoX = Po.X();
                    int PoY = Po.Y();
                    if (PoX == 15 || PoY == 15) { return Chess::Unspecified; }
                    int Shift = PoX * 2;
                    return static_cast<Chess>((Grid[PoY] >> Shift) & Box);
                };
                constexpr void operator =(Chess value) const
                {
                    int PoX = Po.X();
                    int PoY = Po.Y();
                    if (PoX == 15 || PoY == 15) { return; }
                    int Shift = PoX * 2;
					Grid[PoY] &= ~(Box << Shift);
                    Grid[PoY] |= (static_cast<std::uint32_t>(value) & Box) << Shift;
                };
                constexpr Position Pos() const
                {
                    return Po;
                };
            };
            struct ChessCIte
            {
            public:
                friend struct Board;
            private:
                const std::uint32_t* Grid;
                int Index;
                constexpr explicit ChessCIte(const std::uint32_t* Grid, int Index)
                    : Grid{ Grid }, Index{ Index }
                {};
            public:
                constexpr ChessCIte& operator ++()
                {
                    ++Index;
                    return *this;
                };
                constexpr ChessCIte operator ++(int)
                {
                    ChessCIte Result = *this;
                    ++Index;
                    return Result;
                };
                constexpr ChessCRef operator *() const
                {
                    return ChessCRef{ Grid, Position{ Index % 15, Index / 15 } };
                };
                friend static constexpr bool operator ==(const ChessCIte& L, const ChessCIte& R)
                {
                    return L.Grid == R.Grid && L.Index == R.Index;
                };
                friend static constexpr bool operator !=(const ChessCIte& L, const ChessCIte& R)
                {
                    return L.Grid != R.Grid || L.Index != R.Index;
                };
            };
            struct ChessIte
            {
            public:
                friend struct Board;
            private:
                std::uint32_t* Grid;
                int Index;
                constexpr explicit ChessIte(std::uint32_t* Grid, int Index)
                    : Grid{ Grid }, Index{ Index }
                {};
            public:
                constexpr operator ChessCIte() const
                {
                    return ChessCIte{ Grid, Index };
                };
                constexpr ChessIte& operator ++()
                {
                    ++Index;
                    return *this;
                };
                constexpr ChessIte operator ++(int)
                {
                    ChessIte Result = *this;
                    ++Index;
                    return Result;
                };
                constexpr ChessRef operator *() const
                {
                    return ChessRef{ Grid, Position{ Index % 15, Index / 15 } };
                };
                friend static constexpr bool operator ==(const ChessIte& L, const ChessIte& R)
                {
                    return L.Grid == R.Grid && L.Index == R.Index;
                };
                friend static constexpr bool operator !=(const ChessIte& L, const ChessIte& R)
                {
                    return L.Grid != R.Grid || L.Index != R.Index;
                };
            };
        private:
            std::uint32_t Grid[15];
            std::uint8_t Round;
        public:
            constexpr void Reset() &
            {
                for (std::uint32_t& Li : Grid) { Li = 0; }
                Round = 0;
            };
            constexpr ChessCRef operator[](Position Po) const &
            {
                return ChessCRef{ Grid, Po };
            };
            constexpr ChessRef operator[](Position Po) &
            {
                return ChessRef{ Grid, Po };
            };
            constexpr ChessCIte begin() const &
            {
                return ChessCIte{ Grid, 0 };
            };
            constexpr ChessIte begin() &
            {
                return ChessIte{ Grid, 0 };
            };
            constexpr ChessCIte end() const &
            {
                return ChessCIte{ Grid, 225 };
            };
            constexpr ChessIte end() &
            {
                return ChessIte{ Grid, 225 };
            };
            constexpr std::uint32_t GetLine(Position Po, Orientation Or) const &
            {
                std::uint32_t Result = 0u;
                int PoX = Po.X();
                int PoY = Po.Y();
                int ShiftX = PoX * 2;
                if (Or == Orientation::Horizontal)
                {
                    Result = Grid[PoY];
                    if (PoX < 4)
                    {
                        ShiftX = 8 - ShiftX;
                        Result <<= ShiftX;
                        std::uint32_t Mask = ~0u;
                        Mask <<= ShiftX;
                        Result |= ~Mask;
                    }
                    else { Result >>= ShiftX - 8; }
                }
                else
                {
                    int ShiftR = 0;
                    int Step = 0;
                    if (Or == Orientation::Downward)
                    {
                        ShiftX -= 8;
                        Step = 2;
                    }
                    else if (Or == Orientation::Upward)
                    {
                        ShiftX += 8;
                        Step = -2;
                    }
                    for (int Y = PoY - 4, E = PoY + 4; Y <= E; ++Y)
                    {
                        if (Y < 0 || Y > 14 || ShiftX < 0 || ShiftX > 28) { Result |= Box << ShiftR; }
                        else { Result |= ((Grid[Y] >> ShiftX) & Box) << ShiftR; }
                        ShiftX += Step;
                        ShiftR += 2;
                    }
                }
                Result |= static_cast<std::uint32_t>(Or) << 30;
                Result |= 0x3FFC0000u;
                return Result;
            };
            constexpr Board& operator ++()
            {
                if (Round < 225) { ++Round; }
                return *this;
            };
            constexpr Board& operator ++(int) = delete;
            constexpr bool IsBlackTurn() const &
            {
                return Round % 2 == 0;
            };
            constexpr bool IsPending() const &
            {
                return Round == 0;
            };
            constexpr bool IsEnded() const &
            {
                return Round == 225;
            };
        };
        struct Counter
        {
        public:
            struct Pack
            {
            public:
                std::uint32_t Mask;
                std::uint32_t Case;
                constexpr explicit Pack(std::uint32_t Mask, std::uint32_t Case)
                    : Mask{ Mask }, Case{ Case }
                {};
            };
            struct Group
            {
            public:
                int O0;
                int O1;
                int O2;
                int O3;
                constexpr explicit Group(int O0, int O1, int O2, int O3)
                    : O0{ O0 }, O1{ O1 }, O2{ O2 }, O3{ O3 }
                {};
            };
            struct Forbids
            {
            public:
                struct NibbleRef
                {
                private:
                    static constexpr const std::uint32_t Box = 0xFu;
                    std::uint32_t* Po;
                    int i;
                public:
                    constexpr explicit NibbleRef(std::uint32_t* Po, int i)
                        : Po{ Po }, i{ i }
                    {};
                    constexpr operator int() const
                    {
                        int Shift = i * 4;
                        return static_cast<int>((*Po >> Shift) & Box);
                    };
                    constexpr void operator =(int value) const
                    {
                        int Shift = i * 4;
                        *Po &= ~(Box << Shift);
                        *Po |= (static_cast<std::uint32_t>(value) & Box) << Shift;
                    };
                    constexpr void operator +=(int value) const
                    {
                        operator =(operator int() + value);
                    };
                    constexpr void operator -=(int value) const
                    {
                        operator =(operator int() - value);
                    };
                };
            private:
                std::uint32_t Po;
            public:
                constexpr Position P0() const &
                {
                    return static_cast<Position>(Po & Byte);
                };
                constexpr void P0(Position value) &
                {
                    Po &= ~Byte;
                    Po |= static_cast<std::uint32_t>(value);
                };
                constexpr Position P1() const &
                {
                    return static_cast<Position>((Po >> 8) & Byte);
                };
                constexpr void P1(Position value) &
                {
                    Po &= ~(Byte << 8);
                    Po |= static_cast<std::uint32_t>(value) << 8;
                };
                constexpr Position P2() const &
                {
                    return static_cast<Position>((Po >> 16) & Byte);
                };
                constexpr void P2(Position value) &
                {
                    Po &= ~(Byte << 16);
                    Po |= static_cast<std::uint32_t>(value) << 16;
                };
                constexpr Position P3() const &
                {
                    return static_cast<Position>((Po >> 24) & Byte);
                };
                constexpr void P3(Position value) &
                {
                    Po &= ~(Byte << 24);
                    Po |= static_cast<std::uint32_t>(value) << 24;
                };
                constexpr NibbleRef operator[](int i) &
                {
                    return NibbleRef{ &Po, i };
                };
                constexpr int operator[](int i) const &
                {
                    int Shift = i * 4;
                    return static_cast<int>((Po & (0xFu << Shift)) >> Shift);
                };
                constexpr Forbids(Position Po)
                    : Po{ static_cast<std::uint32_t>(Po) }
                {
                    this->Po |= this->Po << 8;
                    this->Po |= this->Po << 16;
                };
            };
            static const Group B3[12];
            static const Pack D4[6];
            static const Pack T3[12];
            static const Pack T4[20];
            static const Pack T5[5];
            static const Pack T6[4];
            Position Po;
            std::uint32_t C3;
            std::uint32_t C4;
            std::uint32_t C5;
            std::uint32_t C6;
            constexpr Counter(Position CurrentPo)
                : Po{ CurrentPo }, C3{ 0u }, C4{ 0u }, C5{ 0u }, C6{ 0u }
            {};
            static constexpr std::uint32_t MySide(std::uint32_t Li, bool Bk)
            {
                std::uint32_t Result = 0;
                for (int Shift = 0; Shift < 18; Shift += 2)
                {
                    std::uint32_t Temp = Li >> Shift;
                    if (!Bk || (Temp & Bit) == ((Temp >> 1) & Bit)) { Temp = ~Temp; }
                    Result |= (Temp & Box) << Shift;
                }
                Result |= Li & 0xC0000000u;
                return Result;
            };
            template <std::size_t N>
            static constexpr bool Exists(std::uint32_t MySide, const Pack(&Cases)[N])
            {
                for (Pack P : Cases)
                {
                    if ((MySide & P.Mask) == P.Case)
                    {
                        return true;
                    }
                }
                return false;
            };
        };
#pragma endregion
#pragma region fields
        private:
            const Params* Pa;
            Position Po;
            Result Re;
            Board Bo;
            UINT BtnReset;
            ULONG_PTR Gdiplus;
            HWND Window;
            bool Dragging;
#pragma endregion
#pragma region constructors-and-methods
        public:
            MainWindow();
            MainWindow(const Params* Pa);
            static void Run(MainWindow& Form);
        private:
            void Constructor() &
            {
                int Outer = Pa->BoardOuterSize();
                HMENU Mu = CreateMenu();
                AppendMenuW(Mu, MF_STRING, BtnReset, Pa->Reset);
                WNDCLASSW Wc{ 0 };
                Wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(COLOR_WINDOW + 1));
                Wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
                HMODULE Mo = NULL;
                GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, L"", &Mo);
                Wc.hInstance = Mo;
                Wc.hIcon = LoadIconW(Mo, MAKEINTRESOURCEW(MAINICON));
                DWORD Id = GetCurrentThreadId();
                std::wstring Name = L"Gomoku" + std::to_wstring(Id);
                Wc.lpszClassName = Name.c_str();
                Wc.lpfnWndProc = WindowProcedure;
                if (!RegisterClassW(&Wc)) { throw std::exception("[Unhandled Exception]"); }
                DWORD Dw = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
                RECT Bd{};
                Bd.left = 0;
                Bd.top = 0;
                Bd.right = Bd.left + Outer;
                Bd.bottom = Bd.top + Outer;
                AdjustWindowRect(&Bd, Dw, TRUE);
                int W = Bd.right - Bd.left;
                int H = Bd.bottom - Bd.top;
                int X = GetSystemMetrics(SM_CXSCREEN) / 2 - W / 2;
                int Y = GetSystemMetrics(SM_CYSCREEN) / 2 - H / 2;
                CreateWindowExW(NULL, Wc.lpszClassName, Params::Game, Dw, X, Y, W, H, NULL, Mu, Wc.hInstance, this);
                MSG Msg{ 0 };
                BOOL Rst{ 0 };
                while ((Rst = GetMessageW(&Msg, NULL, NULL, NULL)) != 0)
                {
                    if (Rst == -1) { break; }
                    TranslateMessage(&Msg);
                    DispatchMessageW(&Msg);
                }
                UnregisterClassW(Wc.lpszClassName, Wc.hInstance);
            };
            void NewGame() &
            {
                Po = Position::Null;
                Re = Result::None;
                Bo.Reset();
                SetWindowTextW(Window, Params::Game);
                HMENU Mu = GetMenu(Window);
                EnableMenuItem(Mu, BtnReset, MF_DISABLED);
                SetMenu(Window, Mu);
                Dragging = false;
            };
            Position GetNearPos(const Gdiplus::Point& Pt) const &
            {
                int Inner = Pa->BoardInnerSize();
                int Bp = Pa->BoardPadding();
                int Lw = Pa->LineWeight();
                int Gs = Pa->GridSize();
                int Begin = Bp - Gs / 2 - Lw / 2;
                int End = Bp + Inner + Gs / 2 + Lw / 2;
                int Step = Gs + Lw;
                if (Pt.X < Begin || Pt.Y < Begin) { return Position::Null; }
                else if (Pt.X > End || Pt.Y > End) { return Position::Null; }
                int X = (Pt.X - Begin) / Step;
                int Y = (Pt.Y - Begin) / Step;
                if (X < 0 || X > 14 || Y < 0 || Y > 14) { return Position::Null; }
                return Position{ X, Y };
            };
            Gdiplus::Point GetPoint(Position Po) const &
            {
                int Bp = Pa->BoardPadding();
                int Lw = Pa->LineWeight();
                int Gs = Pa->GridSize();
                int Begin = Bp + Lw / 2;
                int Step = Gs + Lw;
                int PoX = Po.X();
                int PoY = Po.Y();
                int X = Begin + Step * PoX;
                int Y = Begin + Step * PoY;
                return Gdiplus::Point{ X, Y };
            };
            double GetRadius(Position Po, const Gdiplus::Point& Pt) const &
            {
                Gdiplus::Point PoPt = GetPoint(Po);
                int SqX = PoPt.X - Pt.X;
                int SqY = PoPt.Y - Pt.Y;
                SqX *= SqX;
                SqY *= SqY;
                return std::sqrt(SqX + SqY);
            };
            bool CanPutChess(Position Po) const &
            {
                if (Po == Position::Null) { return false; }
                if (Re != Result::None) { return false; }
                if (Bo.IsEnded()) { return false; }
                int PoX = Po.X();
                int PoY = Po.Y();
                if (Bo.IsPending() && !(PoX == 7 && PoY == 7)) { return false; }
                return Bo[Po] == Chess::None;
            };
            bool CanTouchChess(Position Po, const Gdiplus::Point& Pt) const &
            {
                int CTou = Pa->ChessTouch();
                return GetRadius(Po, Pt) <= CTou / 2e0;
            };
#pragma endregion
#pragma region illustrators
        private:
            Gdiplus::Rect GetChessRect(const Gdiplus::Point& Pt) const &
            {
                int CSiz = Pa->ChessSize();
                if (CSiz % 2 != Pa->LineWeight() % 2) { ++CSiz; }
                Gdiplus::Point Po{ Pt.X - CSiz / 2 - 1, Pt.Y - CSiz / 2 - 1 };
                Gdiplus::Size Sz{ CSiz + 1, CSiz + 1 };
                return Gdiplus::Rect{ Po, Sz };
            };
            void PaintChess(Gdiplus::Graphics& Gr, const Gdiplus::Point& Pt, bool Bk) const &
            {
                int CMar = Pa->ChessMargin();
                int CSha = Pa->ChessShadow();
                Gdiplus::Rect CRect = GetChessRect(Pt);
                Gdiplus::Point SPo{ CRect.X - CMar, CRect.Y - CMar };
                Gdiplus::Size SSz{ CSha, CSha };
                Gdiplus::Rect SRect{ SPo, SSz };
                Gdiplus::GraphicsPath GP{};
                GP.AddEllipse(SRect);
                Gdiplus::PathGradientBrush PGB{ &GP };
                if (Bk)
                {
                    Gdiplus::SolidBrush Brush{ Params::BlackChessDarkColor };
                    Gr.FillEllipse(&Brush, CRect);
                    PGB.SetCenterColor(Params::BlackChessLightColor);
                    Gdiplus::Color Colors[]{ Params::BlackChessDarkColor };
                    INT ColorsCount = 1;
                    PGB.SetSurroundColors(Colors, &ColorsCount);
                    Gr.FillEllipse(&PGB, CRect);
                }
                else
                {
                    Gdiplus::SolidBrush Brush{ Params::WhiteChessDarkColor };
                    Gr.FillEllipse(&Brush, CRect);
                    PGB.SetCenterColor(Params::WhiteChessLightColor);
                    Gdiplus::Color Colors[]{ Params::WhiteChessDarkColor };
                    INT ColorsCount = 1;
                    PGB.SetSurroundColors(Colors, &ColorsCount);
                    Gr.FillEllipse(&PGB, CRect);
                }
            };
            void PaintShadow(Gdiplus::Graphics& Gr, const Gdiplus::Point& Pt) const &
            {
                Gdiplus::Rect CRect = GetChessRect(Pt);
                Gdiplus::SolidBrush ShadowBrush{ Params::ShadowColor };
                Gr.FillEllipse(&ShadowBrush, CRect);
            };
            void ClearGrid(Gdiplus::Graphics& Gr, const Gdiplus::Point& Pt) const &
            {
                Gdiplus::Rect CRect = GetChessRect(Pt);
                Gdiplus::SolidBrush BoardBrush{ Params::BoardColor };
                Gr.FillEllipse(&BoardBrush, CRect);
                Gdiplus::SolidBrush LineBrush{ Params::LineColor };
                Gdiplus::Pen LinePen{ &LineBrush, static_cast<Gdiplus::REAL>(Pa->LineWeight()) };
                int PoX = Po.X();
                int PoY = Po.Y();
                Gdiplus::Point Left{ PoX == 0 ? Pt.X : CRect.GetLeft(), Pt.Y };
                Gdiplus::Point Right{ PoX == 14 ? Pt.X : CRect.GetRight(), Pt.Y };
                Gdiplus::Point Top{ Pt.X, PoY == 0 ? Pt.Y : CRect.GetTop() };
                Gdiplus::Point Bottom{ Pt.X, PoY == 14 ? Pt.Y : CRect.GetBottom() };
                Gr.DrawLine(&LinePen, Left, Right);
                Gr.DrawLine(&LinePen, Top, Bottom);
            };
            void RenderClient(Gdiplus::Graphics& Gr) const &
            {
                Gr.Clear(Params::BoardColor);
                int Inner = Pa->BoardInnerSize();
                int Bp = Pa->BoardPadding();
                int Lw = Pa->LineWeight();
                int Gs = Pa->GridSize();
                int Begin = Bp + Lw / 2;
                int End = Bp + Inner - Lw / 2;
                int Size = Inner - Lw;
                int Step = Gs + Lw;
                Gdiplus::Pen LinePen{ Params::LineColor, static_cast<Gdiplus::REAL>(Lw) };
                Gr.DrawRectangle(&LinePen, Gdiplus::Rect{ Gdiplus::Point{ Begin, Begin }, Gdiplus::Size{ Size, Size } });
                for (int i = 0; i < 15; ++i)
                {
                    int Adjusted = Begin + Step * i;
                    Gr.DrawLine(&LinePen, Gdiplus::Point{ Begin, Adjusted }, Gdiplus::Point{ End, Adjusted });
                    Gr.DrawLine(&LinePen, Gdiplus::Point{ Adjusted, Begin }, Gdiplus::Point{ Adjusted, End });
                }
                for (Board::ChessCRef Ch : Bo)
                {
                    if (Ch == Chess::Black) { PaintChess(Gr, GetPoint(Ch.Pos()), true); }
                    else if (Ch == Chess::White) { PaintChess(Gr, GetPoint(Ch.Pos()), false); }
                }
            };
#pragma endregion
#pragma region referees
        private:
            Counter::Forbids FindForbidPos(std::uint32_t MySide, Position Po) const &
            {
                Orientation Or = static_cast<Orientation>(MySide >> 30);
                Counter::Forbids Fo = Counter::Forbids{ Po };
                for (int i = 0; i < std::extent_v<decltype(Counter::T3)>; ++i)
                {
                    if ((MySide & Counter::T3[i].Mask) == Counter::T3[i].Case)
                    {
                        if (Or == Orientation::Horizontal)
                        {
                            Fo[0] += Counter::B3[i].O0;
                            Fo[2] += Counter::B3[i].O1;
                            Fo[4] += Counter::B3[i].O2;
                            Fo[6] += Counter::B3[i].O3;
                        }
                        else if (Or == Orientation::Vertical)
                        {
                            Fo[1] += Counter::B3[i].O0;
                            Fo[3] += Counter::B3[i].O1;
                            Fo[5] += Counter::B3[i].O2;
                            Fo[7] += Counter::B3[i].O3;
                        }
                        else if (Or == Orientation::Downward)
                        {
                            Fo[0] += Counter::B3[i].O0;
                            Fo[2] += Counter::B3[i].O1;
                            Fo[4] += Counter::B3[i].O2;
                            Fo[6] += Counter::B3[i].O3;
                            Fo[1] += Counter::B3[i].O0;
                            Fo[3] += Counter::B3[i].O1;
                            Fo[5] += Counter::B3[i].O2;
                            Fo[7] += Counter::B3[i].O3;
                        }
                        else if (Or == Orientation::Upward)
                        {
                            Fo[0] -= Counter::B3[i].O0;
                            Fo[2] -= Counter::B3[i].O1;
                            Fo[4] -= Counter::B3[i].O2;
                            Fo[6] -= Counter::B3[i].O3;
                            Fo[1] += Counter::B3[i].O0;
                            Fo[3] += Counter::B3[i].O1;
                            Fo[5] += Counter::B3[i].O2;
                            Fo[7] += Counter::B3[i].O3;
                        }
                        break;
                    }
                }
                return Fo;
            };
            void CountMatches(std::uint32_t Li, Counter& Co) &
            {
                bool Bk = Bo.IsBlackTurn();
                std::uint32_t MySide = Counter::MySide(Li, Bk);
                if (Bk && Counter::Exists(MySide, Counter::T6)) { ++Co.C6; }
                else if (Counter::Exists(MySide, Counter::T5)) { ++Co.C5; }
                else if (Bk && Counter::Exists(MySide, Counter::D4)) { Co.C4 += 2; }
                else if (Bk && Counter::Exists(MySide, Counter::T4)) { ++Co.C4; }
                else if (Bk && Counter::Exists(MySide, Counter::T3))
                {
                    Counter::Forbids Fo = FindForbidPos(MySide, Co.Po);
                    Position P0 = Fo.P0();
                    if (P0 == Co.Po)
                    {
                        if (!(S3Forbid(Fo.P1()) || S3Forbid(Fo.P2()) || S3Forbid(Fo.P3()))) { ++Co.C3; }
                    }
                    else if (!(S3Forbid(P0) || S3Forbid(Fo.P1())) || !(S3Forbid(Fo.P2()) || S3Forbid(Fo.P3()))) { ++Co.C3; }
                }
            };
            bool S3Forbid(Position Po) &
            {
                Counter Co = Counter{ Po };
                if (Bo[Po] == Chess::White || Bo[Po] == Chess::Unspecified) { return true; }
                Bo[Po] = Chess::Black;
                CountMatches(Bo.GetLine(Po, Orientation::Horizontal), Co);
                CountMatches(Bo.GetLine(Po, Orientation::Vertical), Co);
                CountMatches(Bo.GetLine(Po, Orientation::Downward), Co);
                CountMatches(Bo.GetLine(Po, Orientation::Upward), Co);
                Bo[Po] = Chess::None;
                return Co.C3 >= 2u || Co.C4 >= 2u || Co.C5 > 0u || Co.C6 > 0u;
            };
            void MakeResult() &
            {
                bool Bk = Bo.IsBlackTurn();
                Counter Co = Counter{ Po };
                CountMatches(Bo.GetLine(Po, Orientation::Horizontal), Co);
                CountMatches(Bo.GetLine(Po, Orientation::Vertical), Co);
                CountMatches(Bo.GetLine(Po, Orientation::Downward), Co);
                CountMatches(Bo.GetLine(Po, Orientation::Upward), Co);
                if (Bk)
                {
                    if (Co.C3 >= 2u || Co.C4 >= 2u || Co.C6 > 0u) { Re = Result::Lost; }
                    else if (Co.C5 > 0u) { Re = Result::Won; }
                }
                else if (Co.C5 > 0u) { Re = Result::Lost; }
            };
#pragma endregion
#pragma region infrastructures
        private:
            void SetShadow(const Gdiplus::Point& Pt) &
            {
                ReleastShadow(Pt);
                Position CurrentPo = GetNearPos(Pt);
                if (CurrentPo != Po && CanPutChess(CurrentPo) && CanTouchChess(CurrentPo, Pt))
                {
                    Gdiplus::Graphics BoardPaint{ Window };
                    PaintShadow(BoardPaint, GetPoint(CurrentPo));
                    Po = CurrentPo;
                }
            };
            void ReleastShadow(const Gdiplus::Point& Pt) &
            {
                if (CanPutChess(Po) && !CanTouchChess(Po, Pt))
                {
                    Gdiplus::Graphics BoardPaint{ Window };
                    ClearGrid(BoardPaint, GetPoint(Po));
                    Po = Position::Null;
                }
            };
            void PutChess()
            {
                bool Bk = Bo.IsBlackTurn();
                HMENU Mu = GetMenu(Window);
                EnableMenuItem(Mu, BtnReset, MF_ENABLED);
                SetMenu(Window, Mu);
                if (Bk) { Bo[Po] = Chess::Black; }
                else { Bo[Po] = Chess::White; }
                Gdiplus::Graphics BoardPaint{ Window };
                PaintChess(BoardPaint, GetPoint(Po), Bk);
                MakeResult();
                if (Re == Result::Won) { SetWindowTextW(Window, Params::Win); }
                else if (Re == Result::Lost) { SetWindowTextW(Window, Params::Lost); }
                else if (Re == Result::Tied) { SetWindowTextW(Window, Params::Tied); }
                Po = Position::Null;
                ++Bo;
            };
#pragma endregion
#pragma region event-handlers
        private:
            static LRESULT CALLBACK WindowProcedure(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
            {
                MainWindow* ths = reinterpret_cast<MainWindow*>(GetWindowLongPtrW(wnd, GWLP_USERDATA));
                Gdiplus::Point Pt{ LOWORD(lp), HIWORD(lp) };
                switch (msg)
                {
                case WM_LBUTTONDOWN:
                    SetCapture(wnd);
                    {
                        ths->Dragging = true;
                    }
                    break;
                case WM_MOUSEMOVE:
                    if (!ths->Dragging)
                    {
                        ths->SetShadow(Pt);
                    }
                    break;
                case WM_LBUTTONUP:
                    {
                        Position CurrentPo = ths->GetNearPos(Pt);
                        if (CurrentPo == ths->Po && ths->CanPutChess(CurrentPo) && ths->CanTouchChess(CurrentPo, Pt))
                        {
                            ths->PutChess();
                        }
                        ths->Dragging = false;
                    }
                    ReleaseCapture();
                    break;
                case WM_COMMAND:
                    if (wp == ths->BtnReset)
                    {
                        ths->NewGame();
                        Gdiplus::Graphics BoardPaint{ wnd };
                        ths->RenderClient(BoardPaint);
                    }
                    break;
                case WM_CREATE:
                    ths = reinterpret_cast<MainWindow*>(reinterpret_cast<LPCREATESTRUCT>(lp)->lpCreateParams);
                    SetWindowLongPtrW(wnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ths));
                    Gdiplus::GdiplusStartup(&ths->Gdiplus, &Params::StartupInput, nullptr);
                    ths->Window = wnd;
                    ths->NewGame();
                    break;
                case WM_PAINT:
                    {
                        Gdiplus::Graphics BoardPaint{ wnd };
                        ths->RenderClient(BoardPaint);
                    }
                    break;
                case WM_DESTROY:
                    Gdiplus::GdiplusShutdown(ths->Gdiplus);
                    PostQuitMessage(0);
                    break;
                default:
                    break;
                }
                return DefWindowProcW(wnd, msg, wp, lp);
            };
#pragma endregion
    };
#pragma region global-initializes
    constexpr const MainWindow::Params MainWindow::Params::Default{};
    const Gdiplus::Color MainWindow::Params::BoardColor{ 0xFFF5DEB3 };
    const Gdiplus::Color MainWindow::Params::LineColor{ 0xFFDEB887 };
    const Gdiplus::Color MainWindow::Params::ShadowColor{ 0x10000000 };
    const Gdiplus::Color MainWindow::Params::BlackChessLightColor{ 0xFFA0A0A0 };
    const Gdiplus::Color MainWindow::Params::BlackChessDarkColor{ 0xFF000000 };
    const Gdiplus::Color MainWindow::Params::WhiteChessLightColor{ 0xFFFFFFFF };
    const Gdiplus::Color MainWindow::Params::WhiteChessDarkColor{ 0xFFF0F0F0 };
    const Gdiplus::GdiplusStartupInput MainWindow::Params::StartupInput{};
    constexpr const MainWindow::Position MainWindow::Position::Null{ 15, 15 };
    constexpr const MainWindow::Counter::Group MainWindow::Counter::B3[12]{
        Group{ -3, -4, 1, 2 },
        Group{ -3, -4, 1, 2 },
        Group{ -2, -3, 2, 3 },
        Group{ -2, -3, 2, 3 },
        Group{ -1, -2, 3, 4 },
        Group{ -1, -2, 3, 4 },
        Group{ 0, -2, -4, 1 },
        Group{ 0, 1, -2, 3 },
        Group{ 0, -1, -3, 2 },
        Group{ 0, 2, -1, 4 },
        Group{ 0, -1, -4, 1 },
        Group{ 0, 1, -1, 4 },
    };
    constexpr const MainWindow::Counter::Pack MainWindow::Counter::D4[6]{
        Pack{ 0b111111111111111111u, 0b010101110111010101u },
        Pack{ 0b001111111111111111u, 0b000101110101110101u },
        Pack{ 0b111111111111111100u, 0b010111010111010100u },
        Pack{ 0b000011111111111111u, 0b000001110101011101u },
        Pack{ 0b001111111111111100u, 0b000111010101110100u },
        Pack{ 0b111111111111110000u, 0b011101010111010000u },
    };
    constexpr const MainWindow::Counter::Pack MainWindow::Counter::T3[12]{
        Pack{ 0b000000111111111111u, 0b000000110101011111u },
        Pack{ 0b000011111111111100u, 0b000011110101011100u },
        Pack{ 0b000011111111111100u, 0b000011010101111100u },
        Pack{ 0b001111111111110000u, 0b001111010101110000u },
        Pack{ 0b001111111111110000u, 0b001101010111110000u },
        Pack{ 0b111111111111000000u, 0b111101010111000000u },
        Pack{ 0b000000111111111111u, 0b000000110101110111u },
        Pack{ 0b001111111111110000u, 0b001101110101110000u },
        Pack{ 0b000011111111111100u, 0b000011010111011100u },
        Pack{ 0b111111111111000000u, 0b110111010111000000u },
        Pack{ 0b000000111111111111u, 0b000000110111010111u },
        Pack{ 0b111111111111000000u, 0b110101110111000000u },
    };
    constexpr const MainWindow::Counter::Pack MainWindow::Counter::T4[20]{
        Pack{ 0b000000001111111111u, 0b000000000101010111u },
        Pack{ 0b000000111111111100u, 0b000000110101010100u },
        Pack{ 0b000000111111111100u, 0b000000010101011100u },
        Pack{ 0b000011111111110000u, 0b000011010101010000u },
        Pack{ 0b000011111111110000u, 0b000001010101110000u },
        Pack{ 0b001111111111000000u, 0b001101010101000000u },
        Pack{ 0b001111111111000000u, 0b000101010111000000u },
        Pack{ 0b111111111100000000u, 0b110101010100000000u },
        Pack{ 0b000000001111111111u, 0b000000000101011101u },
        Pack{ 0b000011111111110000u, 0b000001110101010000u },
        Pack{ 0b000000111111111100u, 0b000000010101110100u },
        Pack{ 0b001111111111000000u, 0b000111010101000000u },
        Pack{ 0b000011111111110000u, 0b000001010111010000u },
        Pack{ 0b111111111100000000u, 0b011101010100000000u },
        Pack{ 0b000000001111111111u, 0b000000000101110101u },
        Pack{ 0b001111111111000000u, 0b000101110101000000u },
        Pack{ 0b000000111111111100u, 0b000000010111010100u },
        Pack{ 0b111111111100000000u, 0b010111010100000000u },
        Pack{ 0b000000001111111111u, 0b000000000111010101u },
        Pack{ 0b111111111100000000u, 0b010101110100000000u },
    };
    constexpr const MainWindow::Counter::Pack MainWindow::Counter::T5[5]{
        Pack{ 0b000000001111111111u, 0b000000000101010101u },
        Pack{ 0b000000111111111100u, 0b000000010101010100u },
        Pack{ 0b000011111111110000u, 0b000001010101010000u },
        Pack{ 0b001111111111000000u, 0b000101010101000000u },
        Pack{ 0b111111111100000000u, 0b010101010100000000u },
    };
    constexpr const MainWindow::Counter::Pack MainWindow::Counter::T6[4]{
        Pack{ 0b000000111111111111u, 0b000000010101010101u },
        Pack{ 0b000011111111111100u, 0b000001010101010100u },
        Pack{ 0b001111111111110000u, 0b000101010101010000u },
        Pack{ 0b111111111111000000u, 0b010101010101000000u },
    };
    MainWindow::MainWindow()
        : Pa{ &Params::Default }, Po{ Position::Null }, Re{ Result::None }, Bo{}, BtnReset{ 0 }, Gdiplus{ NULL }, Window{ NULL }, Dragging{ false }
    {};
    MainWindow::MainWindow(const Params* Pa)
        : Pa{ Pa }, Po{ Position::Null }, Re{ Result::None }, Bo{}, BtnReset{ 0 }, Gdiplus{ NULL }, Window{ NULL }, Dragging{ false }
    {};
    void MainWindow::Run(MainWindow& Form)
    {
        Form.Constructor();
    };
#pragma endregion
}
