#pragma region license
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
#include <cstdint>
#include <Windows.h>
#include <gdiplus.h>
namespace Gomoku
{
    class MainWindow
    {
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
            constexpr int BoardPadding() const&
            {
                return Bp;
            };
            constexpr int LineWeight() const&
            {
                return Lw;
            };
            constexpr int GridSize() const&
            {
                return Gs;
            };
            constexpr float ChessSizeScale() const&
            {
                return CSiz;
            };
            constexpr float ChessTouchScale() const&
            {
                return CTou;
            };
            constexpr float ChessShadowScale() const&
            {
                return CSha;
            };
            constexpr float ChessMarginScale() const&
            {
                return CMar;
            };
            constexpr Params()
                : Bp{ DefBp }, Lw{ DefLw }, Gs{ DefGs }, CSiz{ DefCSiz }, CTou{ DefCTou }, CSha{ DefCSha }, CMar{ DefCMar }
            {};
            constexpr bool SetParams(int BoardPadding, int LineWeight, int GridSize)&
            {
                if (LineWeight >= GridSize) { return false; }
                else if (GridSize >= BoardPadding) { return false; }
                else if (BoardPadding <= 0) { return false; }
                Bp = BoardPadding;
                Lw = LineWeight;
                Gs = GridSize;
                return true;
            };
            constexpr bool SetScales(float ChessSizeScale, float ChessTouchScale, float ChessShadowScale, float ChessMarginScale)&
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
            constexpr int BoardOuterSize() const&
            {
                return Bp * 2 + Lw * 15 + Gs * 14;
            };
            constexpr int BoardInnerSize() const&
            {
                return Lw * 15 + Gs * 14;
            };
            constexpr int GsScaleToLw(float C) const&
            {
                int Result = static_cast<int>(Gs * C);
                return Lw % 2 == Result % 2 ? Result : Result + 1;
            };
            constexpr int ChessSize() const&
            {
                return GsScaleToLw(CSiz);
            };
            constexpr int ChessTouch() const&
            {
                return GsScaleToLw(CTou);
            };
            constexpr int ChessShadow() const&
            {
                return GsScaleToLw(CSha);
            };
            constexpr int ChessMargin() const&
            {
                return GsScaleToLw(CMar);
            };
        };
    private:
		using Result = std::uint32_t;
        struct Position
        {
        private:
            std::uint32_t Coord;
        };
        struct Board
        {
        private:
            std::uint32_t Grid[15];
            std::uint8_t Round;
		};
        const Params* Pa;
        Position Po;
        Result Re;
        Board Bo;
        UINT BtnReset;
        ULONG_PTR Gdiplus;
        HWND Window;
        bool Dragging;
    public:
        MainWindow();
        MainWindow(const Params* Pa);
        static void Run(MainWindow& Form);
    };
}
int WINAPI wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInst, _In_ LPWSTR args, _In_ int nCmdShow)
{
    Gomoku::MainWindow Form;
    Gomoku::MainWindow::Run(Form);
    return 0;
};
