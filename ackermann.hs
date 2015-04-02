import System.Environment
 
ack :: Int -> Int -> Int
ack 0 n = n + 1
ack m 0 = ack (m-1) 1 
ack m n = ack (m-1) (ack m (n-1))
	 
main = do [m,n] <- getArgs >>= return . map read
          print $ ack m n