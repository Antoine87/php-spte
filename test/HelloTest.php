<?php

declare(strict_types=1);

namespace Antoine87Test\PhpSpte;

use function is_resource;
use function pclose;
use function popen;
use function stream_get_contents;

class HelloTest extends TestCase
{
    /** @var string */
    private static $executable;

    public static function setUpBeforeClass(): void
    {
        parent::setUpBeforeClass();

        self::$executable = self::$binDir . '/hello';
    }

    public function testHello(): void
    {
        $process = popen(self::$executable, 'rb');

        $this->assertTrue(is_resource($process));

        $hello = stream_get_contents($process);
        pclose($process);

        $this->assertSame('Hello !', $hello);

    }
}
