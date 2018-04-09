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
; Code begins now
 NOP
 NOP
 NOP
 ; Initialize the stack
 LDA #$FF
 STA $0400
 STA $0401
 ; global x
 LDA #0
 STA 1026
 ; global y
 LDA #0
 STA 1027
 JMP main
moveRight:
 ; beginfunc moveRight
 ; Allocating 3 for locals
 LDA #253
 CLC
 ADC $0400
 STA $0400
 ; arg amount has stack offset 1
 ; assign $t0 := x
 ; local := global
 LDA 1026
 LDX $0401
 STA 255,X
 ; assign $t1 := amount
 ; local := param
 LDX $0401
 LDA 1,X
 LDX $0401
 STA 254,X
 ; add $t2 := $t0 + $t1
 LDA #0
 STA $0
 LDX $0401
 LDA 255,X
 CLC
 ADC $0
 STA $0
 LDX $0401
 LDA 254,X
 CLC
 ADC $0
 LDX $0401
 STA 253,X
 ; assign x := $t2
 ; global = local
 LDX $0401
 LDA 253,X
 STA 1026
 ; endFunc
 LDA #3
 CLC
 ADC $0400
 STA $0400
 RTS
main:
 ; beginfunc main
 ; Allocating 4 for locals
 LDA #252
 CLC
 ADC $0400
 STA $0400
 ; assign $t3 := 20
 ; local := const
 LDA #20
 LDX $0401
 STA 255,X
 ; assign x := $t3
 ; global = local
 LDX $0401
 LDA 255,X
 STA 1026
 ; assign $t4 := 40
 ; local := const
 LDA #40
 LDX $0401
 STA 254,X
 ; assign y := $t4
 ; global = local
 LDX $0401
 LDA 254,X
 STA 1027
LABEL0:
 ; assign $t5 := 1
 ; local := const
 LDA #1
 LDX $0401
 STA 253,X
 ; branch if $t5 is 0
 LDX $0401
 LDA 253,X
 CMP #0
 BEQ LABEL1
 ;;;;;; NESWAITFORFRAME ;;;;;;
 INC 99
 wait_for_frame:
 LDA 99
 BNE wait_for_frame
 ; assign $t6 := 1
 ; local := const
 LDA #1
 LDX $0401
 STA 252,X
 ; push param $t6
 LDX $0401
 LDA 252,X
 DEC $0400
 LDX $0400
 STA $0,X
 ; push FP, call, restore FP
 LDA $0401
 DEC $0400
 LDX $0400
 STA $0,X
 LDA $0400
 STA $0401
 JSR moveRight
 LDX $0400
 LDA $0,X
 INC $0400
 STA $0401
 ; Pop 1 params
 LDA #1
 CLC
 ADC $0400
 STA $0400
 ;;;;;; NESSETSPRITEX ;;;;;;
 LDA 1026
 STA $0203
 ;;;;;; NESSETSPRITEY ;;;;;;
 LDA 1027
 STA $0200
 ; jump to LABEL0
 JMP LABEL0
LABEL1:
 ; endFunc
 LDA #4
 CLC
 ADC $0400
 STA $0400
 RTS
 
;; ----------------- CODE ENDS HERE -----------------------
; Executed every frame
NMI:
  ; Push AXY
  PHA
  TXA
  PHA
  TYA
  PHA

  ; now the only thing we need to do here is issue a DMA call to transfer our
  ; sprite data into SPR-RAM
  LDA #$00 ; Load 0 into A
  STA $2003 ; reset the SPR-RAM write offset
  LDA #$02 ; Load 2 into A
  STA $4014 ; start the DMA transfer from $0200

  ; Set sleeping to 0
  LDA #$00
  STA sleeping

  ; Pop AXY
  PLA
  TAY
  PLA
  TAX
  PLA

  ; Return
  RTI

palette:
  .db $0F,$30,$0F,$30,$0F,$30,$0F,$30,$0F,$30,$0F,$30,$0F,$30,$0F,$30
  .db $0F,$30,$0F,$30,$0F,$30,$0F,$30,$0F,$30,$0F,$30,$0F,$30,$0F,$30

  .orga $FFFA
  .dw NMI
  .dw RESET
  .dw 0

  .bank 1 slot 1
  .org $0000
  .incbin "sprites.chr"
