
use16
org 0x0100
call main


print_int:
	push bp
	mov bp, sp

	; push word 10

	mov ax, [test_number]
	mov bl, 0

print_count_loop:
	mov dx, 0
	div [ten]
	add bl, 1
	cmp ax, 0
	jne print_count_loop

	mov ch, 0
	mov cl, bl
	mov ax, [test_number]
print_loop:
	mov dx, 0
	div [ten]
	; mov dl, ah

	; push dx
	push ax
	mov ah, 2
	; mov dl, 
	add dl, 48 ; Add position of '0' in ascii
	mov si, int_string_buffer
	add si, cx
	sub si, 1
	mov [si], dl
	; int 0x21
	pop ax

	loop print_loop

	mov si, int_string_buffer
	mov bh, 0
	add si, bx
	mov [si], byte '$'
	mov dx, int_string_buffer
	mov ah, 9
	int 0x21

	; add sp, 2
	pop bp
	ret


main:
	mov dx, msg
	mov ah, 9
	int 0x21

	mov dx, msg2
	mov ah, 9
	int 0x21

	call print_int

	; call game_loop

;input_loop:
;;	call poll_keyboard
;	jmp input_loop

	; Exit
	mov ah, 0x4C
	int 0x21


draw_quad:
	push bp
	mov bp, sp
	add bp, 10 ; Start of parameters

	mov di, [bp-2]

	mov cx, [bp-6]
y_loop:
	mov si, [bp-0]
	push cx
	mov cx, [bp-4]
x_loop:
	push cx
	mov ax, 0x0C0A
	mov cx, si
	mov dx, di
	int 0x10
	pop cx
	add si, 1
	loop x_loop
	pop cx
	add di, 1
	loop y_loop

	pop bp
	ret


init_keyboard:
	mov ah, 1
	int 0x16


poll_keyboard:
	mov ah, 0x01
	int 0x16
	jz buffer_check_end
	mov ah, 0x00
	int 0x16
	;mov dl, ah
	;mov ah, 2
	;int 0x21
buffer_check_end:
	cmp ah, [key_left_code]
	jz set_left
	cmp ah, [key_right_code]
	jz set_right
	jmp poll_end
set_left:
	mov [key_left], 1
	jmp poll_end
set_right:
	mov [key_left], 0
	jmp poll_end
poll_end:
	ret


game_loop:
	; Set video mode
	mov ax, 0x0013
	int 0x10

	mov si, 0
	mov di, 0
frame:
	call poll_keyboard

	cmp [key_left], 0
	jz skip_move
	sub [player_x], 1
skip_move:

	push [player_x]
	push 100
	push 50
	push 10
	call draw_quad
	add sp, 4

	;mov ax, 0x0C0A
	;mov cx, si
	;mov dx, di
	;int 0x10
	;add si, 1
	; add di, 1
	jmp frame

	; Set video back to text mode
	mov ax, 0x0003
	int 0x10

	ret


msg db "Hello DOS!",10,"$"
msg2 db "How are you doing?",10,"$"
msg3 db "SUP DUDE",10,"$"
key_left_code db 'K'
key_right_code db 'M'
key_top_code db 'H'
key_bottom_code db 'P'
key_left db 0
key_right db 0
key_top db 0
key_bottom db 0
player_x dw 200
test_number dw 47921
ten dw 10
int_string_buffer db 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
