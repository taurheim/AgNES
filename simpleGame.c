int sprite_x;
int sprite_y;
int sleeping;

int frame() {
    // Automatically push XYA to stack
    sleeping = 0;
    // Automatically pop XYA from stack
}

int main() {
    // NES will have already done:
    // Reset Interrupts (same every time)
    // Clear memory (same every time)
    // Loaded palettes 
        // This actually has some info in it. We need to know the NES palette 
    // Enable PPU
    sprite_x = 50;
    sprite_y = 50;

    // Game loop
    while(1) {
        if(sleeping == 0) {
            // Game code
        }
    }
}