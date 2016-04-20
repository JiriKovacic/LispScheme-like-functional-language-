# LispScheme-like functional language

# Scheme-like language implementation

This implementation was collaborated with my collegue [Adam Léhar](leharada@fit.cvut.cz).

The task was to implement a new Scheme (or Lisp)-like functional language based on programmable techniques.

##Assignment:
* Program should take its input from a file and print output to another file, specified on the command line. Input and output files should be human-readable/writable. There are no other requirements on the format except that.
* Program must be written in a programming language implemented by yourself, i.e. you must first design and implement a programming language interpreter or compiler and a runtime for it and then implement a solution of one of the above problems in it.
* Compiler itself is not graded, don't spend too much time with it and feel free to use existing bytecode compilers (javac etc.)
* You may use any language you want to implement your programming language runtime.

##Solution
We have written a parser of incoming code in functional language. Then the code was processed and built an abstract syntax tree to generate ourselves defined bytecode. We have implemented own instructions which are compiled into bytecode and at virtual machine the bytecode is interpreted. There is own managed memory with a copying garbage collector. All runs at nested environments and it is managed by Pushdown automaton.

## Example of use
### Simple operations with lambda function (closure)
    (define (a bz fn) (lambda (f) (- bz fn f)))
    (defvar (c (a 55 11)))
    (c 22)
    (c 33)
    (c 44)

    (define (d v) (lambda (f g) (+ v f g)))
    (defvar (e (d 100)))
    (e 20 5)
    (e 30 5)

### Factorial example
    (defvar (a 6))
    (define (fact n) (if (= n 1) 1 (* n (fact (- n 1)))))
    (fact a)
    (a)

### QuickSort example
    define (list-length L)
        (if (= (car L) #nil)
        0
        (+ 1 (list-length (cdr L)))
        )
        )

        (define (item-at-pos L pos)
        (if (< pos 1)
        #nil
        (if (= pos 1)
        (car L)
        (item-at-pos (cdr L) (- pos 1))
        )
        )
        )

        (define (append-list-help L1 L2 pos)
        (if (< pos 1)
        L2
        (append-list-help L1 (cons (item-at-pos L1 pos) L2) (- pos 1))
        )
        )

        (define (append-list L1 L2)
        (append-list-help L1 L2 (list-length L1))
        )

        (define (reverse-list-help L result)
        (if (= (car L) #nil)
        result
        (reverse-list-help (cdr L) (cons (car L) result))
        )
        )

        (define (reverse-list L)
        (reverse-list-help L #nil)
        )

        (define (listLS-help L pivot result)
        (if (= (car L) #nil)
        result
        (if (< (car L) pivot)
        (listLS-help (cdr L) pivot (cons (car L) result))
        (listLS-help (cdr L) pivot result)
        )
        )
        )

        (define (listLS L pivot)
        (reverse-list (listLS-help L pivot #nil))
        )

        (define (listGEQ-help L pivot result)
        (if (= (car L) #nil)
        result
        (if (>= (car L) pivot)
        (listGEQ-help (cdr L) pivot (cons (car L) result))
        (listGEQ-help (cdr L) pivot result)
        )
        )
        )

        (define (listGEQ L pivot)
        (reverse-list (listGEQ-help L pivot #nil))
        )

        (define (quicksort L)
        (if (= (car L) #nil)
        #nil
        (if (= (cdr L) #nil)
        (cons (car L) #nil)
        (append-list (append-list (quicksort (listLS (cdr L) (car L))) (cons (car L) #nil)) (quicksort (listGEQ (cdr L) (car L))))
        )
        )
        )

        (defvar (a (list 4 5 2 1 8 3 1)))

        (quicksort a)
