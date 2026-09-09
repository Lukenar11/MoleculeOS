import System.IO (readLn, hFlush, stdout)
import Text.Printf (printf)  
import Data.Bits (Bits(complement, (.&.)))


alignUp :: Int -> Int -> IO ()
alignUp value alignment = do
    let result = (value + (alignment - 1)) .&. complement (alignment - 1)

    putStrLn "\nalignUp: "
    printf "(%d + (%d - 1)) & ~(%d - 1) = %d\n" value alignment alignment result
    printf "%d `mod` %d = %d\n" result alignment (value `mod` alignment)


alignDown :: Int -> Int -> IO ()
alignDown value alignment = do 
    let result = value .&. complement (alignment - 1)

    putStrLn "\nalignDown: "
    printf "%d & ~(%d - 1) = %d\n" value alignment result
    printf "%d `mod` %d = %d\n" result alignment (value `mod` alignment)


isAlign :: Int -> Int -> IO ()
isAlign value alignment = do
    let result = value `mod` alignment

    putStrLn "\nisAlign"
    printf "%d `mod` %d = %d\n" value alignment result


main :: IO ()
main = do
    putStr "alignment: "
    hFlush stdout
    alignment <- readLn :: IO Int

    putStr "value: "
    hFlush stdout
    value <- readLn :: IO Int

    alignUp value alignment
    alignDown value alignment
    isAlign value alignment
