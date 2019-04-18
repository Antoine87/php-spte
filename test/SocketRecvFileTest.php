<?php

declare(strict_types=1);

namespace Antoine87Test\PhpSpte;

use function fclose;
use function fwrite;
use function is_resource;
use function proc_close;
use function proc_open;
use function stream_socket_client;
use function unlink;

class SocketRecvFileTest extends TestCase
{
    /** @var string */
    private static $executable;

    public static function setUpBeforeClass(): void
    {
        parent::setUpBeforeClass();

        self::$executable = self::$binDir . '/socket-recv-file';
    }

    public function testExecutable(): void
    {
        $descriptorspec = [
            0 => ['pipe', 'rb'], // stdin
            1 => ['file', 'socket-recv-file.log', 'wb'], // stdout
            2 => ['file', 'socket-recv-file.log', 'wb']  // stderr
        ];

        $commandLine = self::$executable . ' 3490 socket-recv-file.dump --stop-first-msg';

        $process = proc_open($commandLine, $descriptorspec, $pipes);

        $this->assertTrue(is_resource($process));

        $socketStream = stream_socket_client('tcp://localhost:3490');
        fwrite($socketStream, 'foo bar');
        fclose($socketStream);

        proc_close($process);

        $this->assertFileExists('socket-recv-file.log');
        $this->assertFileExists('socket-recv-file.dump');
        $this->assertStringEqualsFile('socket-recv-file.dump', 'foo bar');

        unlink('socket-recv-file.log');
        unlink('socket-recv-file.dump');
    }
}
