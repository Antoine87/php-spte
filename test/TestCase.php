<?php

declare(strict_types=1);

namespace Antoine87Test\PhpSpte;

use PHPUnit\Framework\TestCase as BaseTestCase;

class TestCase extends BaseTestCase
{
    /** @var string */
    protected static $oldCwd;

    /** @var string */
    protected static $binDir;

    public static function setUpBeforeClass(): void
    {
        self::$oldCwd = getcwd();
        self::$binDir = dirname(__DIR__) . '/bin';

        chdir(__DIR__ . '/tmp');
    }

    public static function tearDownAfterClass(): void
    {
        chdir(self::$oldCwd);
    }

    protected static function getBinDir(): string
    {
        return dirname(__DIR__) . '/bin';
    }

    protected static function geTmpDir(): string
    {
        return __DIR__ . '/tmp';
    }
}
