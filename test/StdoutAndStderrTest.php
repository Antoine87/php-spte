<?php

declare(strict_types=1);

namespace Antoine87Test\PhpSpte;

use function proc_close;
use function proc_open;

class StdoutAndStderrTest extends TestCase
{
    /** @var string */
    private static $executable;

    public static function setUpBeforeClass(): void
    {
        parent::setUpBeforeClass();

        self::$executable = self::$binDir . '/stdout-and-stderr';
    }

    public function testExecutable(): void
    {
        $descriptorspec = [
            0 => ['pipe', 'rb'], // stdin
            1 => ['pipe', 'wb'], // stdout
            2 => ['pipe', 'wb'], // stderr
        ];

        $process = proc_open(self::$executable, $descriptorspec, $pipes);

        $this->assertTrue(is_resource($process));

        $this->assertSame('Output stream', stream_get_contents($pipes[1]));
        $this->assertSame('Error stream', stream_get_contents($pipes[2]));

        proc_close($process);
    }
}
