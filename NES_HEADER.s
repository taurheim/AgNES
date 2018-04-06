.ROMBANKMAP
BANKSTOTAL  2
BANKSIZE    $4000
BANKS       1
BANKSIZE    $2000
BANKS       1
.ENDRO

.MEMORYMAP
DEFAULTSLOT  0
SLOTSIZE     $4000
SLOT 0       $C000
SLOTSIZE     $2000
SLOT 1       $0000
.ENDME

; just use the actual locations in our copy of SPR-RAM rather than zero-page
; addresses, to avoid having to do multiple copies
.define sprite_x  $0203
.define sprite_y  $0200
.define sleeping  99

  .bank 0 slot 0
  .org $0000
RESET:
  SEI
  CLD
  LDX #$FF
  TXS
  INX
  STX $2000.w
  STX $2001.w
  STX $4010.w
  LDX #$40
  STX $4017.w

vblankwait1:
  BIT $2002
  BPL vblankwait1

clrmem:
  LDA #$00
  STA $0000, x
  STA $0100, x
  STA $0300, x
  STA $0400, x
  STA $0500, x
  STA $0600, x
  STA $0700, x
  LDA #$FE
  STA $0200, x
  INX
  BNE clrmem

  ; start with the sprite near the middle of the screen
  LDA #$00
  STA sprite_x
  STA sprite_y

vblankwait2:
  BIT $2002
  BPL vblankwait2

  ; PPU is initialized here, so we can start writing data into it. this is safe
  ; because we have not yet enabled drawing, and so we don't have to restrain
  ; ourselves to vblank.

  ; first, we load the palettes into $3F00 and $3F10
load_palettes:
  LDA $2002    ; read here so that the next byte written to $2006 is the high
  LDA #$3F     ; byte of the address
  STA $2006    ; write the high byte of the base address
  LDA #$00
  STA $2006    ; write the low byte of the base address
  LDX #$00
load_palettes_loop:
  LDA palette.w, x
  STA $2007
  INX
  CPX #$20     ; 16 byte background palette plus 16 byte sprite palette
  BNE load_palettes_loop

  ; then we draw the background (doing that here because it won't be changing)
  LDA #$20
  STA $2006 ; high byte of the starting address
  LDA #$00
  STA $2006 ; low byte of the starting address
  LDA #$01 ; pattern index 1 is our background tile
  LDX #$04             ; this loop will load $2000-$23FF, which includes the
  LDY #$00             ; attribute table range, but we can just adjust the
load_background_loop:  ; palette to take that into account
  STA $2007
  INY
  BNE load_background_loop
  DEX
  BNE load_background_loop

  ; then we set the unchanging parts of our sprite (the pattern index and the
  ; attributes)
  LDA #$00
  STA $0201 ; pattern index 0 is our sprite
  STA $0202 ; don't need any attributes

  ; enable the PPU
  LDA #%10000000
  STA $2000
  LDA #%00011000
  STA $2001

  ;; ------------ CODE BELOW -----------------
