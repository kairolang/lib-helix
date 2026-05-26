import std::io;
import std::memory;
import std::libc;

class Point {
    var x: i32;
    var y: i32;
}

class <T> Matrix requires Arithmetic in T {
    priv {
        var rows: i32;
        var cols: i32;
        var data: unsafe *T;
    }

    fn Matrix(self, r: i32, c: i32) {
        self.rows = r;
        self.cols = c;
        self.data = std::libc::malloc((self.rows * self.cols) * sizeof(T)) as unsafe *T;
    }

    op + fn add(self, other: &Matrix::<T>) -> Matrix::<T> { // rust like turbofish syntax is only temporary and will be removed in the self hosted compiler
        var result = Matrix::<T>(self.rows, self.cols);
        for (var i: i32 = 0; i < self.rows * self.cols; ++i):
            ...
    
        return result;
    }

    fn print(self) {
        for i in range(self.rows) {
            for j in range(self.cols) {
                ::print(f"({self(i, j)}) ");
            }
        }
    }
}

extend Matrix for Point { // Specialization for Matrix<Point>
    op + fn add(const other: &Matrix::<Point>) -> Matrix::<Point> {
        ...
    }

    fn print() {
        ...
    }
}

fn main() -> i32 {
    var intMatrix = Matrix::<i32>(2, 2); // Matrix of i32s
    intMatrix(0, 0) = 1;
    intMatrix(0, 1) = 2;
    intMatrix.print();

    var pointMatrix = Matrix::<Point>(2, 2); // Specialized Matrix for Point
    pointMatrix(0, 0) = Point{x=1, y=2};
    pointMatrix(0, 1) = Point{x=3, y=4};
    pointMatrix.print();

    var intMatrix2 = Matrix::<i32>(2, 2); // Another Matrix of i32s
    intMatrix2(0, 0) = 2;
    intMatrix2(0, 1) = 3;

    var intMatrixSum = intMatrix + intMatrix2;
    intMatrixSum.print();

    return 0;
}