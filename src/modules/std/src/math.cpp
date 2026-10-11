extern "C" {
	void add(int x, int y, int &result) {
		result = x + y;
	}
	void subtract(int x, int y, int &result) {
		result = x - y;
	}
}