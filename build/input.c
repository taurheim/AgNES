int x, y;

int moveLeft(int amount) {
    x = x - amount;
}

int main() {
    x = 100;
    y = 40;
    while (1){
        NESWAITFORFRAME;
        moveLeft(1);
        NESSETSPRITEX x;
        NESSETSPRITEY y;
    }
}

