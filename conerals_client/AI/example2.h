// example2
namespace example2{
	mt19937 exrd(time(nullptr));
	void Init(){
		return;
	}
	movement Move(){
		int x = exrd() % mapRows + 1;
		int y = exrd() % mapCols + 1;
		int dir = (exrd() & 3);
		return (movement){x, y, dir};
	}
}
