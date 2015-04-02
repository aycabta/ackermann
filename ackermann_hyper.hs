import System.Environment
import List

hyper _ 0 b = b + 1
hyper 2 _ 2 = 4
hyper a 1 b = a + b
hyper a _ 1 = a
hyper a 2 b = a * b
hyper _ _ 0 = 1
hyper 0 _ _ = 0
hyper 1 _ _ = 1
hyper a 3 b = a ^ b
hyper a 4 b = foldr1 (^) (genericReplicate b a)
hyper a n b = foldr1 (flip hyper (n - 1)) (genericReplicate b a)
 
ackermann m n = hyper 2 m (n + 3) - 3

main = do [m,n] <- getArgs >>= return . map read
          print $ ackermann m n
