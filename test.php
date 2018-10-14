<?php
/**
 *  Bubblesort function in PHP
 *
 *  This function takes an unsorted array as input, sorts it, and returns
 *  the output. It only uses normal PHP operations, and does not rely on
 *  any built-in PHP functions or on functions from extensions
 *
 *  @param  array       An unsorted array of integers
 *  @return array       A sorted array
 */
function scripted_bubblesort(array $input)
{
    // number of elements in the array
    $count = count($input);

    // loop through the array
    for ($i = 0; $i < $count; $i++)
    {
        // loop through the elements that were already processed
        for ($j = 1; $j < $count - $i; $j++)
        {
            // move on if smaller
            if ($input[$j-1] <= $input[$j]) continue;

            // swap elements
            $temp = $input[$j];
            $input[$j] = $input[$j-1];
            $input[$j-1] = $temp;
        }
    }

    // done
    return $input;
}




// Multithreading test.
echo multithreading();

// Performance Test
$count = 9000;
$x = array();
for ($i=0; $i<$count; $i++) $x[] = rand(0, 10000);
$start = microtime(true);
$y = native_bubblesort($x);
$native = microtime(true);
$x = scripted_bubblesort($x);
$scripted = microtime(true);
echo("Native:   ".($native - $start)." seconds\n");
echo("Scripted: ".($scripted - $native)." seconds\n");

make_request_with_cpp("http://jsonplaceholder.typicode.com/todos/1");