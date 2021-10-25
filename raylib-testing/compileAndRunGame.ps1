<# Raylib Compile & Run Script #>
<# I should probably make a Make file or something XD #>

gcc <# Compile Game with GCC #> `
    raylib-testing.c <# Entry-Point C File #> `
    -o ./game.exe <# Output File Path #> `
    -O1 -Wall <# Optimizations and Warning Flags #> `
    -L lib/ <# Including Library Path #> `
    -l raylib -l opengl32 -l gdi32 -l winmm <# Including Raylib Libraries #> `
&& `
./game.exe <# Run Game #>