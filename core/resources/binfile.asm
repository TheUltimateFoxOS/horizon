%macro inc_bin 2
	SECTION .rodata
	GLOBAL %1
	GLOBAL %1_size
%1:
	incbin %2
	db 0
	%1_size: dq %1_size - %1
%endmacro

inc_bin default_font, "./resources/zap-light16.psf"
inc_bin logo, "./resources/foxos.bmp"
inc_bin screen_of_death, "./resources/screen_of_death.bmp"

inc_bin boot_animation_frame_0, "./resources/boot_animation/frame-0.bmp"
inc_bin boot_animation_frame_1, "./resources/boot_animation/frame-1.bmp"
inc_bin boot_animation_frame_2, "./resources/boot_animation/frame-2.bmp"
inc_bin boot_animation_frame_3, "./resources/boot_animation/frame-3.bmp"
inc_bin boot_animation_frame_4, "./resources/boot_animation/frame-4.bmp"
inc_bin boot_animation_frame_5, "./resources/boot_animation/frame-5.bmp"
inc_bin boot_animation_frame_6, "./resources/boot_animation/frame-6.bmp"
inc_bin boot_animation_frame_7, "./resources/boot_animation/frame-7.bmp"