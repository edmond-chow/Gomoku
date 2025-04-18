# Gomoku
A game when you are in the first role, black chesses are forbidden to win from
three-three, four-four, and six or more in a row. In every single suspected
survive three row, we may consider any reference points forbidden by any
subsequents with three-three, four-four, five in a row, or six or more in a row.

# Enumerations
Result
* None = 0
* Won = 1
* Lost = 2
* Tied = 3

Chess
* None = 0
* Black = 1
* White = 2
* Unspecified = 3

Player
* Unknown = 0
* Attacker = 1
* Defender = 2
* Empty = 3

Orientation
* Horizontal = 0
* Vertical = 1
* Downward = 2
* Upward = 3

# Encodings
The Position is 32-bit width while the least significant 1 nibble and 2 nibble
reside X and Y respectively and the most significant 3 nibbles are reserved that
shall be zeros. The Board is an array of 32-bit width where the most significant
2 bits are reserved that shall be zeros representing a row of chesses. One chess
or other enumeration mostly within 2-bit, we define a box to field 2-bit. The
offsets to each box from 32-bit width refer to X. The indices to that array
refer to Y. When X or Y is 15, the result obtained from that Board shall be
Unspecified, modify it shall not affect any bits to the array. The Board.GetLine
packs the 9 chesses relative to the Po at center Position on a particular Or
that specify the Orientation into the least significant bits 0 through 17 from a
32-bit width where the most significant 2 bits reside the binary encoding for
that Or, and the least significant bits 18 through 29 are reserved that shall be
set denoted for Unspecified. The Counter is used to count the forbidden matches
in a row on 4 Orientations. The Counter.MySide transfers the Chess encoded into
a Player role while the Orientation is forward from Li with the same offset.

[Black Side] Chess -> Player
* Chess.None(0b00) -> Player.Empty(0b11)
* Chess.Unspecified(0b11) -> Player.Unknown(0b00)
* Chess.Black(0b01) -> Player.Attacker(0b01)
* Chess.White(0b10) -> Player.Defender(0b10)

[White Side] Chess -> Player
* Chess.None(0b00) -> Player.Empty(0b11)
* Chess.Unspecified(0b11) -> Player.Unknown(0b00)
* Chess.Black(0b01) -> Player.Defender(0b10)
* Chess.White(0b10) -> Player.Attacker(0b01)

The result of that transformation shall clear the reserved bits 18 through 29 to
denote Unknown no matter what is preserved in the same range of bits from source
Li. The Counter.Pack packs the Mask and Case in a pair while the Counter.Group
lists 4 possible forbiddens offset relative to the center Position. The
Counter.FindForbidPos uses MySide to extract 4 possible forbiddens to that Po
where we just have to consider the rest when P0 and Po are identical.

The CountMatches counts the forbidden matches in a row from a given Counter. The
S3Forbid allocates a Counter for a suspected survive three to perform multilevel
checks on each Orientation recursively. The MakeResult also allocates a Counter
for normal Black or White chess process which only carry out forbidden rules to
Black side and all six or more in a row counted as C5.
