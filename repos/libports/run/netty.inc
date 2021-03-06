assert_spec x86

set build_components {
	core init
	drivers/timer drivers/nic server/ram_fs server/vfs
	lib/vfs/lxip
}

source ${genode_dir}/repos/base/run/platform_drv.inc
append_platform_drv_build_components

append config {
<config verbose="yes">
	<parent-provides>
		<service name="ROM"/>
		<service name="IRQ"/>
		<service name="IO_MEM"/>
		<service name="IO_PORT"/>
		<service name="PD"/>
		<service name="RM"/>
		<service name="CPU"/>
		<service name="LOG"/>
		<service name="SIGNAL"/>
	</parent-provides>
	<default-route>
		<any-service> <parent/> <any-child/> </any-service>
	</default-route>
	<default caps="100"/>
	<start name="timer">
		<resource name="RAM" quantum="1M"/>
		<provides> <service name="Timer"/> </provides>
	</start>
}

append_platform_drv_config

append config {
	<start name="nic_drv">
		<binary name="} [nic_drv_binary] {"/>
		<resource name="RAM" quantum="4M"/>
		<provides> <service name="Nic"/> </provides>
	</start>

	<start name="socket_fs" caps="200">
		<binary name="vfs"/>
		<resource name="RAM" quantum="32M"/>
		<provides> <service name="File_system"/> </provides>
		<config ld_verbose="yes">
			<vfs>
				<dir name="socket">
					<lxip ip_addr="10.0.2.55" netmask="255.255.255.0" gateway="10.0.2.1" nameserver="8.8.8.8"/>
					<!-- <lxip dhcp="yes"/> -->
				</dir>
			</vfs>
			<default-policy root="/socket" writeable="yes" />
		</config>
	</start>
}

append boot_modules {
	core init timer } [nic_drv_binary] { ram_fs vfs
	ld.lib.so libc.lib.so libm.lib.so posix.lib.so
	libc_resolv.lib.so stdcxx.lib.so libc_pipe.lib.so
	vfs_lxip.lib.so lxip.lib.so
}

append_platform_drv_boot_modules

append qemu_args " -nographic -net nic,model=e1000 -net tap,ifname=tap0,downscript=no,script=no "

# vi: set ft=tcl :
