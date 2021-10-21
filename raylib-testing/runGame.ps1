<# Raylib Compile & Run Script #>

gcc <# Compile Game #> `
    raylib-testing.c <# Entry-Point C File #> `
    -o ./game.exe <# Output File Path #> `
    -O1 -Wall <# Optimizations and Warning Flags #> `
    -L lib/ <# Including Library Path #> `
    -l raylib -l opengl32 -l gdi32 -l winmm <# Including Actual Libraries #> `
&& `
./game.exe <# Run Game #>