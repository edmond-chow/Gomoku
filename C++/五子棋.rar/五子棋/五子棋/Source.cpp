#include <string>
#include <functional>
#include <atomic>
#include <thread>
#include <Windows.h>
#include <gdiplus.h>
#include "res.h"
namespace Gomoku
{
    consteval std::uint32_t color(std::uint8_t r, std::uint8_t g, std::uint8_t b) { return 0xff000000 + (r << 16) + (g << 8) + b; };
    consteval std::uint32_t color(std::uint8_t a, std::uint8_t r, std::uint8_t g, std::uint8_t b) { return (a << 24) + (r << 16) + (g << 8) + b; };
    struct Point
    {
    public:
        const std::int32_t X;
        const std::int32_t Y;
    };
    class BaseForm
    {
    private:
        std::atomic<bool> disposed;
        static constexpr const std::uint8_t WinConst = 5;
    protected:
        static constexpr const std::uint8_t BoardSize = 15;
    private:
        static constexpr const std::uint8_t MaxRound = BoardSize * BoardSize;
    protected:
        static constexpr const std::wstring_view Title = L"五子棋";
        static constexpr const std::wstring_view TitleBlackWin = L"五子棋［黑方勝利］";
        static constexpr const std::wstring_view TitleWhiteWin = L"五子棋［白方勝利］";
        static constexpr const std::wstring_view TitleTied = L"五子棋［和局］";
        static constexpr const std::wstring_view ResetMenuText = L"重置";
        static constexpr const std::uint32_t BoardOffsetColor = color(245, 222, 179);
        static constexpr const std::uint32_t BoardWideColor = color(222, 184, 135);
        static constexpr const std::uint32_t BlackChessLightColor = color(160, 160, 160);
        static constexpr const std::uint32_t BlackChessDarkColor = color(0, 0, 0);
        static constexpr const std::uint32_t WhiteChessLightColor = color(255, 255, 255);
        static constexpr const std::uint32_t WhiteChessDarkColor = color(240, 240, 240);
        static constexpr const std::uint32_t ShadowColor = color(16, 0, 0, 0);
    private:
        static constexpr const std::uint8_t DefaultBoardOffset = 80;
        static constexpr const std::uint8_t DefaultBoardWide = 3;
        static constexpr const std::uint8_t DefaultChessPadding = 40;
        static constexpr const float DefaultChessSizeScale = 0.75F;
        static constexpr const float DefaultChessTouchScale = 0.9F;
        static constexpr const float DefaultChessShadowScale = 1.5F;
        static constexpr const float DefaultChessMarginScale = 0.5F;
    protected:
        const std::uint8_t BoardOffset;
        const std::uint8_t BoardWide;
        std::int32_t BoardClientSizeProperty() const;
        std::int32_t BoardInnerSizeProperty() const;
    protected:
        std::int8_t GetBoard(std::int8_t x, std::int8_t y) const;
        const std::uint8_t ChessPadding;
        const float ChessSizeScale;
        std::int32_t ChessActualSizeProperty() const;
        const float ChessTouchScale;
        std::int32_t ChessActualTouchProperty() const;
        const float ChessShadowScale;
        std::int32_t ChessActualShadowProperty() const;
        const float ChessMarginScale;
        std::int32_t ChessActualMarginProperty() const;
    private:
        std::int8_t Board[BoardSize][BoardSize];
        std::uint8_t Round;
    protected:
        bool IsBlackTurnProperty() const;
        BaseForm* operator&() & = delete;
        const BaseForm* operator&() const& = delete;
        void* operator new(size_t) = delete;
        void operator delete(void*) = delete;
        BaseForm& operator =(const BaseForm&) & = delete;
    public:
        BaseForm(const BaseForm& Form);
        BaseForm();
        BaseForm(std::uint8_t BoardOffset, std::uint8_t BoardWide, std::uint8_t ChessPadding, float ChessSizeScale, float ChessTouchScale, float ChessShadowScale, float ChessMarginScale);
        static void Run(BaseForm& Form);
        static void Run(BaseForm&& Form);
        static constexpr std::pair<void(*)(BaseForm&), void(*)(BaseForm&&)> Run() { return std::pair(static_cast<void(&)(BaseForm&)>(Run), static_cast<void(&)(BaseForm&&)>(Run)); };
        static void RunAsync(BaseForm& Form);
        static void RunAsync(BaseForm&& Form);
        static constexpr std::pair<void(*)(BaseForm&), void(*)(BaseForm&&)> RunAsync() { return std::pair(static_cast<void(&)(BaseForm&)>(RunAsync), static_cast<void(&)(BaseForm&&)>(RunAsync)); };
        friend auto operator <<(std::pair<void(*)(BaseForm&), void(*)(BaseForm&&)>(&Left)(), BaseForm& Form) -> std::pair<void(*)(BaseForm&), void(*)(BaseForm&&)>(&)();
        friend auto operator <<(std::pair<void(*)(BaseForm&), void(*)(BaseForm&&)>(&Right)(), BaseForm&& Form) -> std::pair<void(*)(BaseForm&), void(*)(BaseForm&&)>(&)();
    private:
        void Constructor();
    protected:
        virtual void OnLoad() = 0;
    private:
        class BoardPosition final
        {
        public:
            using CaptureType = BoardPosition;
            static const CaptureType NullCapture;
        private:
            const std::uint8_t XCoordinate : 4;
            const std::uint8_t YCoordinate : 4;
        public:
            constexpr BoardPosition() noexcept : XCoordinate(0xF), YCoordinate(0xF) {};
            constexpr BoardPosition(std::uint8_t XCoordinate, std::uint8_t YCoordinate) noexcept : XCoordinate(XCoordinate & 0xF), YCoordinate(YCoordinate & 0xF) {};
            constexpr BoardPosition(const BoardPosition& BP) noexcept = default;
            constexpr ~BoardPosition() noexcept = default;
            friend constexpr const bool operator ==(const CaptureType& lhs, const CaptureType& rhs) noexcept;
            constexpr const std::uint8_t getX() const noexcept { return XCoordinate; };
            constexpr const std::uint8_t getY() const noexcept { return YCoordinate; };
        };
    public:
        using CaptureType = BoardPosition::CaptureType;
    protected:
        enum class BoardResult : std::int8_t
        {
            Won = 1,
            Lost = -1,
            Tied = 0,
            None = 2,
        };
        void Reset() &;
        void Load() &;
        virtual void OnReset() & = 0;
        void RePaint() &;
        virtual void OnPaint() & = 0;
        BoardPosition GetNearestPosition(const Point& Touch) const;
        bool CanPutChess(const Point& Touch) const;
        Point GetPoint(const BoardPosition& BP) const;
    private:
        BoardPosition PutChess(const Point& Touch) &;
    protected:
        virtual void PaintChess(const BoardPosition& Nearest, bool IsBlackChess) & = 0;
    private:
        using CaseRange = std::function<bool(std::uint8_t i)>;
        using CaseBoard = std::function<std::int8_t(std::uint8_t i, std::uint8_t j)>;
        using CaseArray = std::function<bool(const std::initializer_list<std::int8_t>& data)>;
        using BPCase = std::function<std::uint8_t(const CaseRange& Range, const CaseBoard& Board)>;
        using BPBase = std::function<std::uint8_t(const CaseArray& Array)>;
        using CheckBoard = std::function<std::int8_t(std::int8_t i, std::uint8_t j, std::uint8_t k)>;
        using CheckBlack = std::function<std::int8_t(std::int8_t i, std::uint8_t j)>;
        using CheckForWinProtoType = std::function<BoardResult(const CheckBoard& Board, const CheckBlack& Black)>;
        BPCase BlackCase6C4(const CaseArray& Array) &;
        BPBase BlackBase6C4(const BoardPosition& BP) &;
        BPCase BlackCase5C5(const CaseArray& Array) &;
        BPBase BlackBase5C5(const BoardPosition& BP) &;
        std::uint8_t BlackS4(const BoardPosition& BP) &;
        std::uint8_t BlackD4(const BoardPosition& BP) &;
        std::uint8_t BlackS3(const BoardPosition& BP) &;
        void Result(const BoardResult& BR) &;
        virtual void HasResult(const BoardResult& BR) & = 0;
        CheckForWinProtoType CheckForWinProto(std::int8_t i, std::uint8_t j) &;
        BoardResult CheckForWinCaseFlat(const CheckBoard& Board, const CheckBlack& Black) &;
        BoardResult CheckForWinCaseLean(const CheckBoard& Board, const CheckBlack& Black) &;
        BoardResult CheckForWin(const BoardPosition& BP) &;
    protected:
        void Do(const Point& Point) &;
        void Dispose() &;
        static uintptr_t impl_count();
        static bool impl_array(const std::initializer_list<std::int8_t>& data, const std::initializer_list<std::int8_t>& satisfy);
    };
    std::int32_t BaseForm::BoardClientSizeProperty() const { return BoardOffset * 2 + BoardWide * BoardSize + ChessPadding * (BoardSize - 1); };
    std::int32_t BaseForm::BoardInnerSizeProperty() const { return BoardWide * BoardSize + ChessPadding * (BoardSize - 1); };
    std::int8_t BaseForm::GetBoard(std::int8_t x, std::int8_t y) const { return Board[x][y]; };
    std::int32_t BaseForm::ChessActualSizeProperty() const
    {
        std::int32_t ret = static_cast<std::int32_t>(std::floor(ChessPadding * ChessSizeScale));
        if (BoardWide % 2 == ret % 2) { return ret; }
        else { return --ret; }
    };
    std::int32_t BaseForm::ChessActualTouchProperty() const
    {
        std::int32_t ret = static_cast<std::int32_t>(std::floor(ChessPadding * ChessTouchScale));
        if (BoardWide % 2 == ret % 2) { return ret; }
        else { return --ret; }
    };
    std::int32_t BaseForm::ChessActualShadowProperty() const { return static_cast<std::int32_t>(std::floor(ChessPadding * ChessSizeScale * ChessShadowScale)); };
    std::int32_t BaseForm::ChessActualMarginProperty() const { return static_cast<std::int32_t>(std::floor(ChessPadding * ChessSizeScale * ChessMarginScale)); };
    bool BaseForm::IsBlackTurnProperty() const { return Round % 2 == 0; };
    BaseForm::BaseForm(const BaseForm& Form) :
        disposed(true),
        BoardOffset(Form.BoardOffset), BoardWide(Form.BoardWide), ChessPadding(Form.ChessPadding), ChessSizeScale(Form.ChessSizeScale), ChessTouchScale(Form.ChessTouchScale), ChessShadowScale(Form.ChessShadowScale), ChessMarginScale(Form.ChessMarginScale),
        Board(), Round() {};
    BaseForm::BaseForm() :
        disposed(true),
        BoardOffset(DefaultBoardOffset), BoardWide(DefaultBoardWide), ChessPadding(DefaultChessPadding), ChessSizeScale(DefaultChessSizeScale), ChessTouchScale(DefaultChessTouchScale), ChessShadowScale(DefaultChessShadowScale), ChessMarginScale(DefaultChessMarginScale),
        Board(), Round() {};
    BaseForm::BaseForm(std::uint8_t BoardOffset, std::uint8_t BoardWide, std::uint8_t ChessPadding, float ChessSizeScale, float ChessTouchScale, float ChessShadowScale, float ChessMarginScale) :
        disposed(true),
        BoardOffset(BoardOffset), BoardWide(BoardWide), ChessPadding(ChessPadding), ChessSizeScale(ChessSizeScale), ChessTouchScale(ChessTouchScale), ChessShadowScale(ChessShadowScale), ChessMarginScale(ChessMarginScale),
        Board(), Round() {};
    void BaseForm::Run(BaseForm& Form) { Form.Constructor(); };
    void BaseForm::Run(BaseForm&& Form) { Form.Constructor(); };
    void BaseForm::RunAsync(BaseForm& Form) { std::thread([&Form]() -> void { Run(Form); }).detach(); };
    void BaseForm::RunAsync(BaseForm&& Form) { std::thread([&Form]() -> void { Run(std::move(Form)); }).detach(); };
    auto operator <<(std::pair<void(*)(BaseForm&), void(*)(BaseForm&&)>(&Left)(), BaseForm& Form) -> std::pair<void(*)(BaseForm&), void(*)(BaseForm&&)>(&)()
    {
        Left().first(Form);
        return Left;
    };
    auto operator <<(std::pair<void(*)(BaseForm&), void(*)(BaseForm&&)>(&Right)(), BaseForm&& Form) -> std::pair<void(*)(BaseForm&), void(*)(BaseForm&&)>(&)()
    {
        Right().second(std::move(Form));
        return Right;
    };
    void BaseForm::Constructor()
    {
        if (disposed == false) { throw std::exception("[Unhandled Exception]"); }
        disposed = false;
        OnLoad();
    };
    constexpr const BaseForm::CaptureType BaseForm::CaptureType::NullCapture;
    constexpr const bool operator ==(const BaseForm::CaptureType& lhs, const BaseForm::CaptureType& rhs) noexcept { return lhs.getX() == rhs.getX() && lhs.getY() == rhs.getY(); };
    void BaseForm::Reset() &
    {
        Load();
        RePaint();
    };
    void BaseForm::Load() &
    {
        for (std::uint8_t i = 0; i < BoardSize; ++i)
        {
            for (std::uint8_t j = 0; j < BoardSize; ++j) { Board[i][j] = 0; }
        }
        Round = 0;
        OnReset();
    };
    void BaseForm::RePaint() & { OnPaint(); };
    BaseForm::BoardPosition BaseForm::GetNearestPosition(const Point& Touch) const
    {
        std::int32_t NearestOffset = BoardOffset - ChessPadding / 2;
        std::int32_t NearestWidth = ChessPadding + BoardWide;
        std::uint8_t NearestX = static_cast<std::uint8_t>(((Touch.X - NearestOffset - (Touch.X - NearestOffset) % NearestWidth) / NearestWidth));
        if (NearestX < 0) { NearestX = 0; }
        else if (NearestX > BoardSize - 1) { NearestX = static_cast<std::uint8_t>((BoardSize - 1)); }
        std::uint8_t NearestY = static_cast<std::uint8_t>(((Touch.Y - NearestOffset - (Touch.Y - NearestOffset) % NearestWidth) / NearestWidth));
        if (NearestY < 0) { NearestY = 0; }
        else if (NearestY > BoardSize - 1) { NearestY = static_cast<std::uint8_t>((BoardSize - 1)); }
        return BoardPosition(NearestX, NearestY);
    };
    bool BaseForm::CanPutChess(const Point& Touch) const
    {
        if (Round >= MaxRound) { return false; }
        BoardPosition Nearest = GetNearestPosition(Touch);
        Point NearestPoint = GetPoint(Nearest);
        if (std::floor(std::sqrt(std::pow(NearestPoint.X - Touch.X, 2) + std::pow(NearestPoint.Y - Touch.Y, 2))) > (float)ChessActualTouchProperty() / 2) { return false; }
        if (Round == 0 && !(Nearest.getX() == 7 && Nearest.getY() == 7)) { return false; }
        if (Board[Nearest.getX()][Nearest.getY()] != 0) { return false; }
        return true;
    };
    Point BaseForm::GetPoint(const BoardPosition& BP) const
    {
        std::int32_t offset = static_cast<std::int32_t>(BoardOffset) + static_cast<std::int32_t>(BoardWide) / 2;
        std::int32_t scale = static_cast<std::int32_t>(BoardWide) + static_cast<std::int32_t>(ChessPadding);
        return Point(offset + scale * BP.getX(), offset + scale * BP.getY());
    };
    BaseForm::BoardPosition BaseForm::PutChess(const Point& Touch) &
    {
        BoardPosition Nearest = GetNearestPosition(Touch);
        if (IsBlackTurnProperty() == true) { Board[Nearest.getX()][Nearest.getY()] = 1; }
        else { Board[Nearest.getX()][Nearest.getY()] = -1; }
        PaintChess(Nearest, IsBlackTurnProperty());
        return Nearest;
    };
    BaseForm::BPCase BaseForm::BlackCase6C4(const CaseArray& Array) &
    {
        return [this, Array](const CaseRange& Range, const CaseBoard& Board) -> BPCase::result_type
        {
            if (IsBlackTurnProperty() == false) { return 0; }
            for (std::uint8_t i = 0; i < 4; ++i)
            {
                if (Range(i)) { continue; }
                if (Array({ Board(i, 0), Board(i, 1), Board(i, 2), Board(i, 3), Board(i, 4), Board(i, 5) })) { return 1; }
            }
            return 0;
        };
    };
    BaseForm::BPBase BaseForm::BlackBase6C4(const BoardPosition& BP) &
    {
        return [this, BP](const CaseArray& Array) -> BPBase::result_type
        {
            if (IsBlackTurnProperty() == false) { return 0; }
            if (BP.getX() > BoardSize - 1 || BP.getX() < 0) { return 0; }
            if (BP.getY() > BoardSize - 1 || BP.getY() < 0) { return 0; }
            return BlackCase6C4(Array)
            (
                [BP](std::uint8_t i) -> CaseRange::result_type { return BP.getX() - i - 1 < 0 || BP.getX() - i + 4 > BoardSize - 1; },
                [this, BP](std::uint8_t i, std::uint8_t j) -> CaseBoard::result_type { return Board[BP.getX() - i + j - 1][BP.getY()]; }
            )
            + BlackCase6C4(Array)
            (
                [BP](std::uint8_t i) -> CaseRange::result_type { return BP.getY() - i - 1 < 0 || BP.getY() - i + 4 > BoardSize - 1; },
                [this, BP](std::uint8_t i, std::uint8_t j) -> CaseBoard::result_type { return Board[BP.getX()][BP.getY() - i + j - 1]; }
            )
            + BlackCase6C4(Array)
            (
                [BP](std::uint8_t i) -> CaseRange::result_type { return BP.getX() - i - 1 < 0 || BP.getX() - i + 4 > BoardSize - 1 || BP.getY() - i - 1 < 0 || BP.getY() - i + 4 > BoardSize - 1; },
                [this, BP](std::uint8_t i,  std::uint8_t j) -> CaseBoard::result_type { return Board[BP.getX() - i + j - 1][BP.getY() - i + j - 1]; }
            )
            + BlackCase6C4(Array)
            (
                [BP](std::uint8_t i) -> CaseRange::result_type { return BP.getX() + i - 4 < 0 || BP.getX() + i + 1 > BoardSize - 1 || BP.getY() - i - 1 < 0 || BP.getY() - i + 4 > BoardSize - 1; },
                [this, BP](std::uint8_t i, std::uint8_t j) -> CaseBoard::result_type { return Board[BP.getX() + i - j + 1][BP.getY() - i + j - 1]; }
            );
        };
    };
    BaseForm::BPCase BaseForm::BlackCase5C5(const CaseArray& Array) &
    {
        return [this, Array](const CaseRange& Range, const CaseBoard& Board) -> BPCase::result_type
        {
            if (IsBlackTurnProperty() == false) { return 0; }
            for (std::uint8_t i = 0; i < 5; ++i)
            {
                if (Range(i)) { continue; }
                if (Array({ Board(i, 0), Board(i, 1), Board(i, 2), Board(i, 3), Board(i, 4) })) { return 1; }
            }
            return 0;
        };
    };
    BaseForm::BPBase BaseForm::BlackBase5C5(const BoardPosition& BP) &
    {
        return [this, BP](const CaseArray& Array) -> BPBase::result_type
        {
            if (IsBlackTurnProperty() == false) { return 0; }
            if (BP.getX() > BoardSize - 1 || BP.getX() < 0) { return 0; }
            if (BP.getY() > BoardSize - 1 || BP.getY() < 0) { return 0; }
            return BlackCase5C5(Array)
            (
                [BP](std::int32_t i) -> CaseRange::result_type { return BP.getX() - i < 0 || BP.getX() - i + 4 > BoardSize - 1; },
                [this, BP](std::uint8_t i, std::uint8_t j) -> CaseBoard::result_type { return Board[BP.getX() - i + j][BP.getY()]; }
            )
            + BlackCase5C5(Array)
            (
                [BP](std::int32_t i) -> CaseRange::result_type { return BP.getY() - i < 0 || BP.getY() - i + 4 > BoardSize - 1; },
                [this, BP](std::uint8_t i, std::uint8_t j) -> CaseBoard::result_type { return Board[BP.getX()][BP.getY() - i + j]; }
            )
            + BlackCase5C5(Array)
            (
                [BP](std::int32_t i) -> CaseRange::result_type { return BP.getX() - i < 0 || BP.getX() - i + 4 > BoardSize - 1 || BP.getY() - i < 0 || BP.getY() - i + 4 > BoardSize - 1; },
                [this, BP](std::uint8_t i, std::uint8_t j) -> CaseBoard::result_type { return Board[BP.getX() - i + j][BP.getY() - i + j]; }
            )
            + BlackCase5C5(Array)
            (
                [BP](std::int32_t i) -> CaseRange::result_type { return BP.getX() + i - 4 < 0 || BP.getX() + i > BoardSize - 1 || BP.getY() - i < 0 || BP.getY() - i + 4 > BoardSize - 1; },
                [this, BP](std::uint8_t i, std::uint8_t j) -> CaseBoard::result_type { return Board[BP.getX() + i - j][BP.getY() - i + j]; }
            );
        };
    };
    std::uint8_t BaseForm::BlackS4(const BoardPosition& BP) &
    {
        if (IsBlackTurnProperty() == false) { return 0; }
        return BlackBase6C4(BP)([](const std::initializer_list<std::int8_t>& data) -> BPBase::result_type { return impl_array(data, { 0, 1, 1, 1, 1, 0 }); });
    };
    std::uint8_t BaseForm::BlackD4(const BoardPosition& BP) &
    {
        if (IsBlackTurnProperty() == false) { return 0; }
        return BlackBase6C4(BP)([](const std::initializer_list<std::int8_t>& data) -> BPBase::result_type { return impl_array(data, { -1, 1, 1, 1, 1, 0 }); })
             + BlackBase6C4(BP)([](const std::initializer_list<std::int8_t>& data) -> BPBase::result_type { return impl_array(data, { 0, 1, 1, 1, 1, -1 }); })
             + BlackBase5C5(BP)([](const std::initializer_list<std::int8_t>& data) -> BPBase::result_type { return impl_array(data, { 1, 0, 1, 1, 1 }); })
             + BlackBase5C5(BP)([](const std::initializer_list<std::int8_t>& data) -> BPBase::result_type { return impl_array(data, { 1, 1, 0, 1, 1 }); })
             + BlackBase5C5(BP)([](const std::initializer_list<std::int8_t>& data) -> BPBase::result_type { return impl_array(data, { 1, 1, 1, 0, 1 }); });
    };
    std::uint8_t BaseForm::BlackS3(const BoardPosition& BP) &
    {
        if (IsBlackTurnProperty() == false) { return 0; }
        return BlackBase6C4(BP)([](const std::initializer_list<std::int8_t>& data) -> BPBase::result_type { return impl_array(data, { 0, 0, 1, 1, 1, 0 }) || impl_array(data, { 0, 1, 1, 1, 0, 0 }); })
             + BlackBase6C4(BP)([](const std::initializer_list<std::int8_t>& data) -> BPBase::result_type { return impl_array(data, { 0, 1, 0, 1, 1, 0 }) || impl_array(data, { 0, 1, 1, 0, 1, 0 }); });
    };
    void BaseForm::Result(const BoardResult& BR) &
    {
        ++Round;
        if (BR != BoardResult::None) { Round = MaxRound + 1U; }
        HasResult(BR);
    };
    BaseForm::CheckForWinProtoType BaseForm::CheckForWinProto(std::int8_t i, std::uint8_t j) &
    {
        return [i, j](const CheckBoard& Board, const CheckBlack & Black) -> CheckForWinProtoType::result_type
        {
            std::int8_t WinCount = 0;
            for (std::uint8_t k = 0; k < WinConst; ++k) { WinCount += Board(i, j, k); }
            if (WinCount == WinConst)
            {
                if (Black(i, j) == 1) { return BoardResult::Lost; }
                return BoardResult::Won;
            }
            else if (WinCount == -WinConst) { return BoardResult::Lost; }
            return BoardResult::None;
        };
    };
    BaseForm::BoardResult BaseForm::CheckForWinCaseFlat(const CheckBoard& Board, const CheckBlack& Black) &
    {
        for (std::int8_t i = 0; i < BoardSize - 1; ++i)
        {
            for (std::uint8_t j = 0; j < BoardSize - (WinConst - 1); ++j)
            {
                BoardResult BR = CheckForWinProto(i, j)(Board, Black);
                if (BR != BoardResult::None) { return BR; }
            }
        }
        return BoardResult::None;
    };
    BaseForm::BoardResult BaseForm::CheckForWinCaseLean(const CheckBoard& Board, const CheckBlack& Black) &
    {
        for (std::int8_t i = (-(BoardSize - WinConst + 1)); i < BoardSize - WinConst + 1; ++i)
        {
            for (std::uint8_t j = 0; j < BoardSize - std::abs(i) - WinConst + 1; ++j)
            {
                BoardResult BR = CheckForWinProto(i, j)(Board, Black);
                if (BR != BoardResult::None) { return BR; }
            }
        }
        return BoardResult::None;
    };
    BaseForm::BoardResult BaseForm::CheckForWin(const BoardPosition& BP) &
    {
        if (Round > MaxRound) { return BoardResult::None; }
        if ((BlackS4(BP) + BlackD4(BP) >= 2 || BlackS3(BP) >= 2) && IsBlackTurnProperty() == true) { return BoardResult::Lost; }
        BoardResult Array[]{
            CheckForWinCaseFlat(
                [this](std::int8_t i, std::int8_t j, std::int8_t k) -> CheckBoard::result_type { return Board[i][j + k]; },
                [this](std::int8_t i, std::int8_t j) -> CheckBlack::result_type { return Board[i][j + WinConst]; }
            ),
            CheckForWinCaseFlat(
                [this](std::int8_t i, std::int8_t j, std::int8_t k) -> CheckBoard::result_type { return Board[j + k][i]; },
                [this](std::int8_t i, std::int8_t j) -> CheckBlack::result_type { return Board[j + WinConst][i]; }
            ),
            CheckForWinCaseLean(
                [this](std::int8_t i, std::int8_t j, std::int8_t k) -> CheckBoard::result_type { return Board[std::max<std::int8_t>(i, 0) + j + k][std::abs(std::min<std::int8_t>(i, 0)) + j + k]; },
                [this](std::int8_t i, std::int8_t j) -> CheckBlack::result_type { return Board[std::max<std::int8_t>(i, 0) + j + WinConst][std::abs(std::min<std::int8_t>(i, 0)) + j + WinConst]; }
            ),
            CheckForWinCaseLean(
                [this](std::int8_t i, std::int8_t j, std::int8_t k) -> CheckBoard::result_type { return Board[std::min<std::int8_t>(BoardSize - 1 - i, BoardSize - 1) - j - k][std::abs(std::min<std::int8_t>(i, 0)) + j + k]; },
                [this](std::int8_t i, std::int8_t j) -> CheckBlack::result_type { return Board[std::min<std::int8_t>(BoardSize - 1 - i, BoardSize - 1) - j - WinConst][std::abs(std::min<std::int8_t>(i, 0)) + j + WinConst]; }
            ),
        };
        for (BoardResult Case : Array)
        {
            if (Case != BoardResult::None) { return Case; }
        }
        if (Round == MaxRound) { return BoardResult::Tied; }
        return BoardResult::None;
    };
    void BaseForm::Do(const Point& Point) &
    {
        if (CanPutChess(Point)) { Result(CheckForWin(PutChess(Point))); }
    };
    void BaseForm::Dispose() & { this->disposed = true; };
    std::uintptr_t BaseForm::impl_count()
    {
        static thread_local const std::uintptr_t count = std::hash<std::thread::id>()(std::this_thread::get_id());
        return count;
    };
    bool BaseForm::impl_array(const std::initializer_list<std::int8_t>& data, const std::initializer_list<std::int8_t>& satisfy)
    {
        if (data.size() != satisfy.size()) { return false; }
        for (std::initializer_list<std::int8_t>::const_iterator ite_data = data.begin(), ite_satisfy = satisfy.begin(); true; ++ite_data, ++ite_satisfy)
        {
            if (*ite_data != *ite_satisfy) { return false; }
        }
        return true;
    };
    class MainForm final : public BaseForm
    {
    private:
        static constexpr const UINT_PTR hMenuResetID = 1;
        HWND hWnd;
        HMENU hMenu;
        const Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;
        CaptureType CaptureBP;
        bool CaptureHold;
        void OnLoad() override;
        void OnReset() & override;
        void OnPaint() & override;
        void PaintChess(const CaptureType& Nearest, bool IsBlackChess) & override;
        void HasResult(const BoardResult& BR) & override;
        void SetShadow(Point Capture);
        void ReleastShadow();
        static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
    public:
        MainForm(const MainForm& Form);
        MainForm();
        MainForm(std::uint8_t BoardOffset, std::uint8_t BoardWide, std::uint8_t ChessPadding, float ChessSizeScale, float ChessTouchScale, float ChessShadowScale, float ChessMarginScale);
    };
    void MainForm::OnLoad()
    {
        hMenu = CreateMenu();
        AppendMenuW(hMenu, MF_STRING, hMenuResetID, ResetMenuText.data());
        WNDCLASSW wc{ 0 };
        wc.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW + 1);
        wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
        HMODULE phModule = NULL;
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, L"", &phModule);
        wc.hInstance = phModule;
        wc.hIcon = LoadIconW(phModule, MAKEINTRESOURCEW(MAINICON));
        std::wstring ClassName = L"Gomoku" + std::to_wstring(impl_count());
        wc.lpszClassName = ClassName.c_str();
        wc.lpfnWndProc = WindowProcedure;
        if (!RegisterClassW(&wc)) { throw std::exception("[Unhandled Exception]"); }
        DWORD dwStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
        RECT win32Border{};
        win32Border.left = 0;
        win32Border.top = 0;
        win32Border.right = win32Border.left + BoardClientSizeProperty();
        win32Border.bottom = win32Border.top + BoardClientSizeProperty();
        AdjustWindowRect(&win32Border, dwStyle, TRUE);
        int SizeX = win32Border.right - win32Border.left;
        int SizeY = win32Border.bottom - win32Border.top;
        int PositionX = GetSystemMetrics(SM_CXSCREEN) / 2 - SizeX / 2;
        int PositionY = GetSystemMetrics(SM_CYSCREEN) / 2 - SizeY / 2;
        CreateWindowExW(0, wc.lpszClassName, Title.data(), dwStyle, PositionX, PositionY, SizeX, SizeY, NULL, hMenu, wc.hInstance, this);
        MSG msg{ 0 };
        BOOL ret = GetMessageW(&msg, NULL, NULL, NULL);
        while (ret != -1)
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            if (ret == 0) { break; }
            ret = GetMessageW(&msg, NULL, NULL, NULL);
        }
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
    };
    void MainForm::OnReset() &
    {
        SetWindowTextW(hWnd, Title.data());
        EnableMenuItem(hMenu, hMenuResetID, MF_DISABLED);
        SetMenu(hWnd, hMenu);
    };
    void MainForm::OnPaint() &
    {
        Gdiplus::Graphics Paint(hWnd);
        Paint.Clear(BoardOffsetColor);
        const std::int32_t ResetOffset = BoardOffset + BoardWide / 2;
        const std::int32_t ResetDistance = BoardWide + ChessPadding;
        Gdiplus::Pen Pen(BoardWideColor, (Gdiplus::REAL)BoardWide);
        Paint.DrawRectangle(&Pen, Gdiplus::Rect(Gdiplus::Point(ResetOffset, ResetOffset), Gdiplus::Size(BoardInnerSizeProperty() - BoardWide, BoardInnerSizeProperty() - BoardWide)));
        for (std::uint8_t i = 0; i < BoardSize; ++i)
        {
            Paint.DrawLine(&Pen, Gdiplus::Point(ResetOffset + ResetDistance * i, ResetOffset), Gdiplus::Point(ResetOffset + ResetDistance * i, BoardOffset + BoardInnerSizeProperty()));
            Paint.DrawLine(&Pen, Gdiplus::Point(ResetOffset, ResetOffset + ResetDistance * i), Gdiplus::Point(BoardOffset + BoardInnerSizeProperty(), ResetOffset + ResetDistance * i));
        }
        for (std::uint8_t i = 0; i < BoardSize; ++i)
        {
            for (std::uint8_t j = 0; j < BoardSize; ++j)
            {
                switch (GetBoard(i, j))
                {
                case 1:
                    PaintChess(CaptureType(i, j), true);
                    break;
                case -1:
                    PaintChess(CaptureType(i, j), false);
                    break;
                default:
                    break;
                }
            }
        }
    };
    void MainForm::PaintChess(const CaptureType& Nearest, bool IsBlackChess) &
    {
        Point NearestPoint = GetPoint(Nearest);
        int32_t ChessOriginPointOffset = -BoardWide / 2 - (ChessActualSizeProperty() - BoardWide) / 2 - 1;
        Gdiplus::Point ChessOriginPoint(NearestPoint.X + ChessOriginPointOffset, NearestPoint.Y + ChessOriginPointOffset);
        Gdiplus::Size ChessOriginSize(ChessActualSizeProperty() + 1, ChessActualSizeProperty() + 1);
        Gdiplus::Rect ChessOriginRect = Gdiplus::Rect(ChessOriginPoint, ChessOriginSize);
        int32_t ChessGPPointOffset = -ChessActualMarginProperty() - 1;
        Gdiplus::Point ChessGPPoint(ChessOriginRect.X + ChessGPPointOffset, ChessOriginRect.Y + ChessGPPointOffset);
        Gdiplus::Size ChessGPSize(ChessActualShadowProperty() + 1, ChessActualShadowProperty() + 1);
        Gdiplus::Rect ChessGPRect = Gdiplus::Rect(ChessGPPoint, ChessGPSize);
        Gdiplus::GraphicsPath GP;
        GP.AddEllipse(ChessGPRect);
        Gdiplus::PathGradientBrush PGB(&GP);
        Gdiplus::Graphics BoardPaint(hWnd);
        if (IsBlackChess == true)
        {
            Gdiplus::SolidBrush Brush(BlackChessDarkColor);
            BoardPaint.FillEllipse(&Brush, ChessOriginRect);
            PGB.SetCenterColor(BlackChessLightColor);
            Gdiplus::Color Colors[]{ BlackChessDarkColor };
            INT ColorsCount = 1;
            PGB.SetSurroundColors(Colors, &ColorsCount);
            BoardPaint.FillEllipse(&PGB, ChessOriginRect);
        }
        else
        {
            Gdiplus::SolidBrush Brush(WhiteChessDarkColor);
            BoardPaint.FillEllipse(&Brush, ChessOriginRect);
            PGB.SetCenterColor(WhiteChessLightColor);
            Gdiplus::Color Colors[]{ WhiteChessDarkColor };
            INT ColorsCount = 1;
            PGB.SetSurroundColors(Colors, &ColorsCount);
            BoardPaint.FillEllipse(&PGB, ChessOriginRect);
        }
    };
    void MainForm::HasResult(const BoardResult& BR) &
    {
        EnableMenuItem(hMenu, hMenuResetID, MF_ENABLED);
        SetMenu(hWnd, hMenu);
        if (BR == BoardResult::None) { return; }
        if (BR == BoardResult::Won) { SetWindowTextW(hWnd, TitleBlackWin.data()); }
        else if (BR == BoardResult::Lost) { SetWindowTextW(hWnd, TitleWhiteWin.data()); }
        else { SetWindowTextW(hWnd, TitleTied.data()); }
    }
    void MainForm::SetShadow(Point Capture)
    {
        CaptureType Nearest = GetNearestPosition(Capture);
        if (CaptureBP != Nearest)
        {
            ReleastShadow();
            if (CanPutChess(Capture))
            {
                Point NearestPoint = GetPoint(Nearest);
                std::int32_t ChessNearestPointOffset = -BoardWide / 2 - (ChessActualSizeProperty() - BoardWide) / 2 - 1;
                Gdiplus::Point ChessNearestPoint(NearestPoint.X + ChessNearestPointOffset, NearestPoint.Y + ChessNearestPointOffset);
                Gdiplus::Size ChessNearestSize(ChessActualSizeProperty() + 1, ChessActualSizeProperty() + 1);
                Gdiplus::Rect ChessNearestRect(ChessNearestPoint, ChessNearestSize);
                Gdiplus::Graphics BoardPaint(hWnd);
                Gdiplus::SolidBrush Blush(ShadowColor);
                BoardPaint.FillEllipse(&Blush, ChessNearestRect);
                CaptureBP.~CaptureType();
                new (&CaptureBP) CaptureType(Nearest);
            }
        }
    };
    void MainForm::ReleastShadow()
    {
        if (CaptureBP != CaptureType::NullCapture)
        {
            Point CapturePoint = GetPoint(CaptureBP);
            std::int32_t ChessCapturePointOffset = -BoardWide / 2 - (ChessActualSizeProperty() - BoardWide) / 2 - 1;
            Gdiplus::Point ChessCapturePoint(CapturePoint.X + ChessCapturePointOffset, CapturePoint.Y + ChessCapturePointOffset);
            Gdiplus::Size ChessCaptureSize(ChessActualSizeProperty() + 1, ChessActualSizeProperty() + 1);
            Gdiplus::Rect ChessCaptureRect(ChessCapturePoint, ChessCaptureSize);
            Gdiplus::Graphics BoardPaint(hWnd);
            Gdiplus::SolidBrush Blush(BoardOffsetColor);
            BoardPaint.FillEllipse(&Blush, ChessCaptureRect);
            Gdiplus::Pen Pen(BoardWideColor, (Gdiplus::REAL)BoardWide);
            BoardPaint.DrawLine(&Pen, Gdiplus::Point(CaptureBP.getX() == 0 ? CapturePoint.X : ChessCaptureRect.GetLeft(), CapturePoint.Y), Gdiplus::Point(CaptureBP.getX() == BoardSize - 1 ? CapturePoint.X : ChessCaptureRect.GetRight(), CapturePoint.Y));
            BoardPaint.DrawLine(&Pen, Gdiplus::Point(CapturePoint.X, CaptureBP.getY() == 0 ? CapturePoint.Y : ChessCaptureRect.GetTop()), Gdiplus::Point(CapturePoint.X, CaptureBP.getY() == BoardSize - 1 ? CapturePoint.Y : ChessCaptureRect.GetBottom()));
            CaptureBP.~CaptureType();
            new (&CaptureBP) CaptureType(CaptureType::NullCapture);
        }
    };
    LRESULT CALLBACK MainForm::WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
    {
        MainForm* ths = reinterpret_cast<MainForm*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
        switch (msg)
        {
        case WM_MOUSEMOVE:
            if (ths->CaptureHold == false) { ths->SetShadow(Point(LOWORD(lp), HIWORD(lp))); }
            break;
        case WM_LBUTTONDOWN:
            SetCapture(hWnd);
            ths->CaptureHold = true;
            break;
        case WM_LBUTTONUP:
            if (ths->CaptureBP == ths->GetNearestPosition(Point(LOWORD(lp), HIWORD(lp))))
            {
                ths->Do(Point(LOWORD(lp), HIWORD(lp)));
                ths->CaptureBP.~CaptureType();
                new (&(ths->CaptureBP)) CaptureType(CaptureType::NullCapture);
            }
            ths->ReleastShadow();
            ths->CaptureHold = false;
            ReleaseCapture();
            break;
        case WM_PAINT:
            ths->RePaint();
            break;
        case WM_COMMAND:
            switch (wp)
            {
            case hMenuResetID:
                ths->Reset();
                break;
            default:
                break;
            }
            break;
        case WM_CREATE:
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)(ths = (MainForm*)((LPCREATESTRUCT)lp)->lpCreateParams));
            Gdiplus::GdiplusStartup(&(ths->gdiplusToken), &(ths->gdiplusStartupInput), nullptr);
            ths->hWnd = hWnd;
            ths->Load();
            break;
        case WM_DESTROY:
            Gdiplus::GdiplusShutdown(ths->gdiplusToken);
            PostQuitMessage(0);
            ths->Dispose();
            break;
        default:
            break;
        }
        return DefWindowProcW(hWnd, msg, wp, lp);
    };
    MainForm::MainForm(const MainForm& Form) :
        BaseForm::BaseForm(Form),
        hWnd(nullptr), hMenu(nullptr), gdiplusStartupInput(), gdiplusToken(),
        CaptureBP(BaseForm::CaptureType::NullCapture), CaptureHold(false) {};
    MainForm::MainForm() :
        BaseForm::BaseForm(),
        hWnd(nullptr), hMenu(nullptr), gdiplusStartupInput(), gdiplusToken(),
        CaptureBP(BaseForm::CaptureType::NullCapture), CaptureHold(false) {};
    MainForm::MainForm(std::uint8_t BoardOffset, std::uint8_t BoardWide, std::uint8_t ChessPadding, float ChessSizeScale, float ChessTouchScale, float ChessShadowScale, float ChessMarginScale) :
        BaseForm::BaseForm(BoardOffset, BoardWide, ChessPadding, ChessSizeScale, ChessTouchScale, ChessShadowScale, ChessMarginScale),
        hWnd(nullptr), hMenu(nullptr), gdiplusStartupInput(), gdiplusToken(),
        CaptureBP(BaseForm::CaptureType::NullCapture), CaptureHold(false) {};
}
int WINAPI wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInst, _In_ LPWSTR args, _In_ int nCmdShow)
{
    Gomoku::MainForm::Run << Gomoku::MainForm::MainForm();
    return 0;
};
