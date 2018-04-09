
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
