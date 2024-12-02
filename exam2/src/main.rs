use std::io;

fn main() {
    let mut input = String::new();
    let rows;
    let cols;

    // 사용자로부터 행렬의 크기 입력 받기
    println!("행렬의 행과 열의 크기를 입력하세요 (예: 2 3):");
    io::stdin().read_line(&mut input).unwrap();
    let dims: Vec<usize> = input
        .trim()
        .split_whitespace()
        .map(|x| x.parse().unwrap())
        .collect();

    if dims.len() == 2 {
        rows = dims[0];
        cols = dims[1];
    } else {
        println!("잘못된 입력입니다. 행렬의 크기를 올바르게 입력하세요.");
        return;
    }

    // 행렬 초기화
    let mut matrix_a: Vec<Vec<i32>> = vec![vec![0; cols]; rows];
    let mut matrix_b: Vec<Vec<i32>> = vec![vec![0; cols]; rows];
    let mut result_matrix: Vec<Vec<i32>> = vec![vec![0; cols]; rows];

    // 첫 번째 행렬 입력 받기
    println!("첫 번째 행렬의 요소를 입력하세요:");
    for i in 0..rows {
        for j in 0..cols {
            print!("A[{}][{}]: ", i, j);
            io::Write::flush(&mut io::stdout()).unwrap(); // 출력 즉시 flush
            let mut element = String::new();
            io::stdin().read_line(&mut element).unwrap();
            matrix_a[i][j] = element.trim().parse().unwrap();
        }
    }

    // 두 번째 행렬 입력 받기
    println!("두 번째 행렬의 요소를 입력하세요:");
    for i in 0..rows {
        for j in 0..cols {
            print!("B[{}][{}]: ", i, j);
            io::Write::flush(&mut io::stdout()).unwrap(); // 출력 즉시 flush
            let mut element = String::new();
            io::stdin().read_line(&mut element).unwrap();
            matrix_b[i][j] = element.trim().parse().unwrap();
        }
    }

    // 행렬 더하기
    for i in 0..rows {
        for j in 0..cols {
            result_matrix[i][j] = matrix_a[i][j] + matrix_b[i][j];
        }
    }

    // 결과 행렬 출력
    println!("두 행렬의 합은 다음과 같습니다:");
    for row in result_matrix {
        for val in row {
            print!("{} ", val);
        }
        println!();
    }
}
