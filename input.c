int x, y;

int main() {
    x = 20;
    y = 40;
    while (1){
        NESWAITFORFRAME;
        x = x + 1;
        y = x;
        NESSETSPRITEX x;
        NESSETSPRITEY y;
    }
}

