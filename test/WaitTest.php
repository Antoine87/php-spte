<?php

declare(strict_types=1);

namespace Antoine87Test\PhpSpte;

use function microtime;
use function is_resource;
use function pclose;
use function popen;

class WaitTest extends TestCase
{
    /** @var string */
    private static $executable;

    public static function setUpBeforeClass(): void
    {
        parent::setUpBeforeClass();

        self::$executable = self::$binDir . '/wait';
    }

    public function testExecutable(): void
    {
        $t1 = microtime(true);

        $commandLine = self::$executable . ' 500';

        $process = popen($commandLine, 'rb');

        $this->assertTrue(is_resource($process));
        pclose($process);

        $t2 = microtime(true);

        $this->assertEqualsWithDelta($t2 - $t1, 0.500, 0.15);
    }
}
