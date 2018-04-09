int x, y;

int moveRight(int amount) {
    x = x + amount;
}

int main() {
    x = 20;
    y = 40;
    while (1){
        NESWAITFORFRAME;
        moveRight(1);
        NESSETSPRITEX x;
        NESSETSPRITEY y;
    }
}

