<?php

declare(strict_types=1);

namespace Antoine87Test\PhpSpte;

use function fclose;
use function fwrite;
use function is_resource;
use function proc_close;
use function proc_open;
use function stream_get_contents;

class StdinToStdoutTest extends TestCase
{
    /** @var string */
    private static $executable;

    public static function setUpBeforeClass(): void
    {
        parent::setUpBeforeClass();

        self::$executable = self::$binDir . '/stdin-to-stdout';
    }

    public function testExecutable(): void
    {
        $descriptorspec = [
            0 => ['pipe', 'rb'], // stdin
            1 => ['pipe', 'wb'], // stdout
            2 => ['pipe', 'wb']  // stderr
        ];

        $process = proc_open(self::$executable, $descriptorspec, $pipes);

        $this->assertTrue(is_resource($process));

        fwrite($pipes[0], 'foobar');
        fclose($pipes[0]);

        $this->assertSame('foobar', stream_get_contents($pipes[1]));
        fclose($pipes[1]);

        proc_close($process);
    }

}
