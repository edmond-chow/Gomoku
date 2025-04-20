# Gomoku
A game when you are in the first role, Black Chesses are Forbidden to Win from
three-three, four-four, and six or more in a row. In every single Suspected
Survive Three row, we shall consider any reference point Forbidden by any
subsequent with three-three, four-four, five in a row, or six or more in a row.

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
The Position is 32-bit width while the 1st and 2nd least significant nibbles
reside X and Y respectively and the most significant 3 nibbles are reserved that
shall be zeros. The Board is an array of 32-bit width where the most significant
2 bits are reserved that shall be zeros representing a row of Chesses. One Chess
or other enumeration merely within 2 bits, we define a box to field 2 bits. The
offsets to each box from 32-bit width refer to X. The indices to that array
refer to Y. When X or Y is 15, the result obtained from that Board shall be
Unspecified, modify it shall not affect any bits to the array. The Board.GetLine
packs those 9 Chesses relative to the Po at center Position on a particular Or
that specify the Orientation into the least significant 18 bits from a 32-bit
width where the most significant 2 bits reside the binary encoding for that Or,
and the 18th through 29th least significant bits are reserved that shall be set
denoted for Unspecified. The Counter is used to count the Forbidden matches in a
row on 4 Orientations. The Counter.MySide transfers the Chesses encoded into
Player roles while the Orientation is forward from Li with the same offset.

Black Side Transformation
* Chess.None(0b00) -> Player.Empty(0b11)
* Chess.Unspecified(0b11) -> Player.Unknown(0b00)
* Chess.Black(0b01) -> Player.Attacker(0b01)
* Chess.White(0b10) -> Player.Defender(0b10)

White Side Transformation
* Chess.None(0b00) -> Player.Empty(0b11)
* Chess.Unspecified(0b11) -> Player.Unknown(0b00)
* Chess.Black(0b01) -> Player.Defender(0b10)
* Chess.White(0b10) -> Player.Attacker(0b01)

The result of that transformation shall clear the reserved bits to denote
Unknown no matter what is preserved in the same range of bits from source Li.
The Counter.Pack packs the Mask and Case in a pair while the Counter.Group lists
4 possible Forbidden offsets relative to the center Position. The FindForbidPos
uses MySide to extract 4 possible Forbidden offsets to that Po and returns a
Counter.Forbids as a result where we just have to consider 3 Positions when P0
and Po are identical.

The CountMatches counts the Forbidden matches in a row from a given Counter. The
S3Forbid allocates a Counter for a Suspected Survive Three to perform multilevel
checks on each Orientation recursively. The MakeResult also allocates a Counter
for normal Black or White Chess process, which only carry out Forbidden rules to
Black side.
